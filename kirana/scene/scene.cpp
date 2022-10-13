#include "scene.hpp"

#include <assimp/scene.h>
#include <algorithm>
#include <constants.h>
#include <logger.hpp>

typedef kirana::utils::Logger Logger;
typedef kirana::utils::LogSeverity LogSeverity;
namespace constants = kirana::utils::constants;

const kirana::scene::Object *kirana::scene::Scene::findObject(
    const aiNode *node) const
{
    if (node != nullptr)
    {
        auto it = std::find_if(m_objects.begin(), m_objects.end(),
                               [&node](const std::shared_ptr<Object> &o) {
                                   return o->getName() == node->mName.C_Str();
                               });

        if (it != m_objects.end())
            return (*it).get();
    }
    return nullptr;
}

void kirana::scene::Scene::getMeshesFromNode(
    const aiNode *node, std::vector<std::shared_ptr<Mesh>> *nodeMeshes)
{
    nodeMeshes->clear();
    nodeMeshes->resize(node->mNumMeshes);
    for (size_t i = 0; i < node->mNumMeshes; i++)
        (*nodeMeshes)[i] = m_meshes[node->mMeshes[i]];
}

void kirana::scene::Scene::initializeChildObjects(
    std::shared_ptr<Object> parent, uint32_t childCount, aiNode **children)
{
    for (size_t i = 0; i < childCount; i++)
    {
        std::vector<std::shared_ptr<Mesh>> meshes;
        getMeshesFromNode(children[i], &meshes);
        m_objects.emplace_back(std::make_shared<Object>(
            children[i], std::move(meshes), parent->m_transform));

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

    // Create Mesh objects for all the meshes in the scene.
    m_meshes.clear();
    m_meshes.resize(scene->mNumMeshes);
    for (size_t i = 0; i < scene->mNumMeshes; i++)
        m_meshes[i] = std::move(std::make_shared<Mesh>(scene->mMeshes[i]));

    Logger::get().log(constants::LOG_CHANNEL_SCENE, LogSeverity::debug,
                      "Scene Mesh count: " + std::to_string(scene->mNumMeshes));

    // Initialize root Object of the scene. First finds all the meshes belonging
    // to the root object.
    std::vector<std::shared_ptr<Mesh>> nodeMeshes;
    getMeshesFromNode(scene->mRootNode, &nodeMeshes);
    m_rootObject = std::make_shared<Object>(scene->mRootNode,
                                            std::move(nodeMeshes), nullptr);

    Logger::get().log(constants::LOG_CHANNEL_SCENE, LogSeverity::debug,
                      "Root Object: " +
                          std::string(scene->mRootNode->mName.C_Str()));

    // Recursively initialize child objects of the scene.
    m_objects.clear();
    m_objects.emplace_back(m_rootObject);
    initializeChildObjects(m_rootObject, scene->mRootNode->mNumChildren,
                           scene->mRootNode->mChildren);

    Logger::get().log(constants::LOG_CHANNEL_SCENE, LogSeverity::debug,
                      "Object count: " + std::to_string(m_objects.size()));
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
