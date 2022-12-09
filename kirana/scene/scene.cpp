#include "scene.hpp"

#include "material.hpp"
#include "primitives/plane.hpp"


#include <assimp/scene.h>
#include <algorithm>
#include <constants.h>
#include <logger.hpp>

typedef kirana::utils::Logger Logger;
typedef kirana::utils::LogSeverity LogSeverity;
namespace constants = kirana::utils::constants;

void kirana::scene::Scene::getMeshesFromNode(
    const aiNode *node, std::vector<std::shared_ptr<Mesh>> *nodeMeshes,
    math::Bounds3 *bounds)
{
    nodeMeshes->clear();
    if (node->mNumMeshes > 0)
    {
        nodeMeshes->resize(node->mNumMeshes);
        for (size_t i = 0; i < node->mNumMeshes; i++)
        {
            (*nodeMeshes)[i] = m_meshes[node->mMeshes[i]];
            bounds->encapsulate((*nodeMeshes)[i]->getBounds());
        }
    }
    else
    {
        // When node has no meshes, it's an empty node with just a position.
        const auto &t = node->mTransformation;
        bounds->encapsulate(math::Vector3(t.a4, t.b4, t.c4));
    }
}

void kirana::scene::Scene::initializeChildObjects(
    std::shared_ptr<Object> parent, uint32_t childCount, aiNode **children)
{

    for (size_t i = 0; i < childCount; i++)
    {
        std::vector<std::shared_ptr<Mesh>> meshes;
        math::Bounds3 objectBounds;
        getMeshesFromNode(children[i], &meshes, &objectBounds);

        m_objects.emplace_back(std::make_shared<Object>(
            children[i], meshes, objectBounds,
            parent != nullptr ? parent->m_transform.get() : nullptr));

        if(parent != nullptr)
            parent->m_addHierarchyBounds(objectBounds);

        if (children[i]->mNumChildren > 0)
            initializeChildObjects(m_objects.back(), children[i]->mNumChildren,
                                   children[i]->mChildren);
    }
}

void kirana::scene::Scene::initFromAiScene(const aiScene *scene)
{
    // Initialize scene name
    if (scene->mName.length > 0)
        m_name = scene->mName.C_Str();

    Logger::get().log(constants::LOG_CHANNEL_SCENE, LogSeverity::debug,
                      "Loading Scene: " + std::string(scene->mName.C_Str()));

    // TODO: Populate scene cameras

    // Create Material objects for all the materials in the scene.
    m_materials.clear();
    m_materials.resize(scene->mNumMaterials);
    for (size_t i = 0; i < scene->mNumMaterials; i++)
    {
        m_materials[i] = std::move(std::make_shared<Material>(
            scene->mMaterials[i]->GetName().C_Str(),
            constants::DEFAULT_SCENE_MATERIAL_SHADER_NAME,
            Material::MaterialProperties{}));
    }

    Logger::get().log(constants::LOG_CHANNEL_SCENE, LogSeverity::debug,
                      "Material count: " + std::to_string(m_materials.size()));

    // Create Mesh objects for all the meshes in the scene.
    m_meshes.clear();
    m_meshes.resize(scene->mNumMeshes);
    for (size_t i = 0; i < scene->mNumMeshes; i++)
        m_meshes[i] = std::move(std::make_shared<Mesh>(
            scene->mMeshes[i], m_materials[scene->mMeshes[i]->mMaterialIndex]));

    Logger::get().log(constants::LOG_CHANNEL_SCENE, LogSeverity::debug,
                      "Scene Mesh count: " + std::to_string(scene->mNumMeshes));

    // Recursively initialize child objects of the scene, starting with the root
    // node.
    m_objects.clear();
    aiNode *nodes[1] = {scene->mRootNode};
    initializeChildObjects(nullptr, 1, nodes);

    Logger::get().log(constants::LOG_CHANNEL_SCENE, LogSeverity::debug,
                      "Object count: " + std::to_string(m_objects.size()));

    m_isInitialized = true;
}


std::vector<kirana::math::Transform *> kirana::scene::Scene::
    getTransformsForMesh(const Mesh *const mesh) const
{
    std::vector<kirana::math::Transform *> transforms;
    for (const auto &o : m_objects)
    {
        if (o->hasMesh(mesh))
            transforms.push_back(o->transform);
    }
    return transforms;
}
