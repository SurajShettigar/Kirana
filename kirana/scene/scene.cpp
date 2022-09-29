#include "scene.hpp"

#include <assimp/scene.h>
#include <algorithm>
#include <constants.h>
#include <logger.hpp>

typedef kirana::utils::Logger Logger;
typedef kirana::utils::LogSeverity LogSeverity;
namespace constants = kirana::utils::constants;

std::shared_ptr<kirana::scene::Object> &kirana::scene::Scene::findObject(
    const aiNode *node)
{
    if (node != nullptr)
    {
        if (m_rootObject->name == node->mName.C_Str())
            return m_rootObject;
        auto it = std::find_if(m_childObjects.begin(), m_childObjects.end(),
                               [&node](const std::shared_ptr<Object> &o) {
                                   return o->name == node->mName.C_Str();
                               });

        if (it != m_childObjects.end())
            return *it;
    }
}

void kirana::scene::Scene::getMeshesFromNode(
    const aiNode *node, std::vector<std::shared_ptr<Mesh>> *nodeMeshes)
{
    nodeMeshes->clear();
    for (size_t i = 0; i < node->mNumMeshes; i++)
        nodeMeshes->push_back(
            m_meshes[node->mMeshes[i]]); // We use push_back instead of
                                         // emplace_back to increase the shared
                                         // pointer count. Both Scene and Object
                                         // will own the Mesh object.
}

void kirana::scene::Scene::initializeChildObjects(
    const std::shared_ptr<Object> &parent, uint32_t childCount,
    aiNode **children)
{
    for (size_t i = 0; i < childCount; i++)
    {
        std::vector<std::shared_ptr<Mesh>> meshes;
        getMeshesFromNode(children[i], &meshes);
        m_childObjects.emplace_back(
            std::make_shared<Object>(children[i], std::move(meshes), parent));

        if (children[i]->mNumChildren > 0)
            initializeChildObjects(m_childObjects[i], children[i]->mNumChildren,
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
    for (size_t i = 0; i < scene->mNumMeshes; i++)
        m_meshes.emplace_back(std::make_shared<Mesh>(scene->mMeshes[i]));

    Logger::get().log(constants::LOG_CHANNEL_SCENE, LogSeverity::debug,
                      "Scene Mesh count: " + std::to_string(scene->mNumMeshes));


    // Initialize root Object of the scene. First finds all the meshes belonging
    // to the root object.
    std::vector<std::shared_ptr<Mesh>> nodeMeshes;
    getMeshesFromNode(scene->mRootNode, &nodeMeshes);
    m_rootObject =
        std::make_shared<Object>(scene->mRootNode, std::move(nodeMeshes));

    Logger::get().log(constants::LOG_CHANNEL_SCENE, LogSeverity::debug,
                      "Root Object: " +
                          std::string(scene->mRootNode->mName.C_Str()));

    // Recursively initialize child objects of the scene.
    m_childObjects.clear();
    initializeChildObjects(m_rootObject, scene->mRootNode->mNumChildren,
                           scene->mRootNode->mChildren);

    Logger::get().log(constants::LOG_CHANNEL_SCENE, LogSeverity::debug,
                      "Child Object count: " +
                          std::to_string(m_childObjects.size()));

}