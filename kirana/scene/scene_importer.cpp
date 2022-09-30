#include "scene_importer.hpp"

#include <logger.hpp>
#include <constants.h>

#include <assimp/DefaultLogger.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>

using kirana::utils::Logger;
using kirana::utils::LogSeverity;
namespace constants = kirana::utils::constants;

uint32_t kirana::scene::SceneImporter::getPostProcessMask(
    const SceneImportSettings &importSettings)
{
    uint32_t mask = 0;
    if (importSettings.calculateTangentSpace)
        mask |= aiProcess_CalcTangentSpace;
    if (importSettings.joinIdenticalVertices)
        mask |= aiProcess_JoinIdenticalVertices;
    if (importSettings.triangulate)
        mask |= aiProcess_Triangulate;
    if (importSettings.generateNormals)
        mask |= aiProcess_GenNormals;
    if (importSettings.generateSmoothNormals)
        mask |= aiProcess_GenSmoothNormals;
    if (importSettings.improveCacheLocality)
        mask |= aiProcess_ImproveCacheLocality;
    if(importSettings.preTransformVertices)
        mask |= aiProcess_PreTransformVertices;
    if(importSettings.optimizeMesh)
        mask |= aiProcess_OptimizeMeshes;

    return mask;
}

bool kirana::scene::SceneImporter::loadSceneFromFile(
    const char *path, const SceneImportSettings &importSettings, Scene  *scene)
{
    Assimp::Importer importer;

    const aiScene *aiScene =
        importer.ReadFile(path, getPostProcessMask(importSettings));

    if (aiScene == nullptr)
    {
        Logger::get().log(constants::LOG_CHANNEL_SCENE, LogSeverity::error,
                          "Failed to open scene from path: " +
                              std::string(path));
        return false;
    }

    scene->initFromAiScene(aiScene);
    return true;
}