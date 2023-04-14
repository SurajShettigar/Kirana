#include "assimp_scene_loader.hpp"

#include "../scene_types.hpp"
#include "../scene.hpp"

#include <assimp/DefaultLogger.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "assimp_scene_converter.hpp"

uint32_t kirana::scene::external::AssimpSceneLoader::getPostProcessMask(
    const SceneImportSettings &settings)
{
    uint32_t mask = 0;
    if (settings.calculateTangentSpace)
        mask |= aiProcess_CalcTangentSpace;
    if (settings.joinIdenticalVertices)
        mask |= aiProcess_JoinIdenticalVertices;
    if (settings.triangulate)
        mask |= aiProcess_Triangulate;
    if (settings.generateNormals)
        mask |= aiProcess_GenNormals;
    if (settings.generateSmoothNormals)
        mask |= aiProcess_GenSmoothNormals;
    if (settings.improveCacheLocality)
        mask |= aiProcess_ImproveCacheLocality;
    if (settings.preTransformVertices)
        mask |= aiProcess_PreTransformVertices;
    if (settings.optimizeMesh)
        mask |= aiProcess_OptimizeMeshes;
    if (settings.generateBoundingBoxes)
        mask |= aiProcess_GenBoundingBoxes;
    if (settings.generateUVs)
        mask |= aiProcess_GenUVCoords;
    if (settings.transformUVs)
        mask |= aiProcess_TransformUVCoords;
    if (settings.flipUVs)
        mask |= aiProcess_FlipUVs;

    return mask;
}

bool kirana::scene::external::AssimpSceneLoader::load(
    const std::string &path, const SceneImportSettings &settings, Scene *scene)
{
    Assimp::Importer importer;

    const aiScene *aiScene =
        importer.ReadFile(path, getPostProcessMask(settings));

    if (aiScene == nullptr)
        return false;

    return AssimpSceneConverter::get().convertScene(aiScene, scene);
}