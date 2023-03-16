#include "assimp_converter.h"


#include <assimp/scene.h>
#include "../scene.hpp"

bool kirana::scene::converters::AssimpConverter::convertTransform(
    const void *inputTransform, math::Transform *outputTransform)
{
    const auto &mat = *reinterpret_cast<const aiMatrix4x4 *>(inputTransform);

    *outputTransform = math::Transform(math::Matrix4x4(
        mat.a1, mat.a2, mat.a3, mat.a4, mat.b1, mat.b2, mat.b3, mat.b4, mat.c1,
        mat.c2, mat.c3, mat.c4, mat.d1, mat.d2, mat.d3, mat.d4));

    return true;
}

bool kirana::scene::converters::AssimpConverter::convertMesh(
    const void *inputMesh, scene::Mesh *outputMesh)
{
    const auto &mesh = *reinterpret_cast<const aiMesh *>(inputMesh);

    outputMesh->m_name = mesh.mName.C_Str();

    // TODO: Add support for other primitive types.
    if ((mesh.mPrimitiveTypes & aiPrimitiveType::aiPrimitiveType_TRIANGLE) == 0)
        return false;

    outputMesh->m_vertices.resize(mesh.mNumVertices);
    for (uint32_t fIndex = 0; fIndex < mesh.mNumFaces; fIndex++)
    {
        const aiFace &face = mesh.mFaces[fIndex];
        for (uint32_t i = 0; i < 3; i++)
        {
            const INDEX_TYPE index = face.mIndices[i];
            outputMesh->m_indices.push_back(index);

            outputMesh->m_vertices[index].position[0] = mesh.mVertices[index].x;
            outputMesh->m_vertices[index].position[1] = mesh.mVertices[index].y;
            outputMesh->m_vertices[index].position[2] = mesh.mVertices[index].z;

            if (mesh.HasNormals())
            {
                outputMesh->m_vertices[index].normal[0] =
                    mesh.mNormals[index].x;
                outputMesh->m_vertices[index].normal[1] =
                    mesh.mNormals[index].y;
                outputMesh->m_vertices[index].normal[2] =
                    mesh.mNormals[index].z;
            }

            if (mesh.HasVertexColors(0))
            {
                outputMesh->m_vertices[index].color[0] =
                    mesh.mColors[0][index].r;
                outputMesh->m_vertices[index].color[1] =
                    mesh.mColors[0][index].g;
                outputMesh->m_vertices[index].color[2] =
                    mesh.mColors[0][index].b;
                outputMesh->m_vertices[index].color[3] =
                    mesh.mColors[0][index].a;
            }

            if (mesh.HasTextureCoords(0))
            {
                outputMesh->m_vertices[index].texCoords[0] =
                    mesh.mTextureCoords[0][index].x;
                outputMesh->m_vertices[index].texCoords[1] =
                    mesh.mTextureCoords[0][index].y;
            }
        }
    }
    outputMesh->m_bounds = math::Bounds3(
        math::Vector3(mesh.mAABB.mMin.x, mesh.mAABB.mMin.y, mesh.mAABB.mMin.z),
        math::Vector3(mesh.mAABB.mMax.x, mesh.mAABB.mMax.y, mesh.mAABB.mMax.z));

    return true;
}

bool kirana::scene::converters::AssimpConverter::convertMaterial(
    const void *inputMaterial, scene::Material *outputMaterial)
{
    const auto &material = *reinterpret_cast<const aiMaterial *>(inputMaterial);

    outputMaterial->m_name = material.GetName().C_Str();
    return true;
}

bool kirana::scene::converters::AssimpConverter::convertLight(
    const void *inputLight, scene::Light *outputLight)
{
    const auto &light = *reinterpret_cast<const aiLight *>(inputLight);

    outputLight->m_name = light.mName.C_Str();
    return true;
}

bool kirana::scene::converters::AssimpConverter::convertCamera(
    const void *inputCamera, scene::Camera *outputCamera)
{
    const auto &camera = *reinterpret_cast<const aiCamera *>(inputCamera);

    outputCamera->m_name = camera.mName.C_Str();
    return true;
}

bool kirana::scene::converters::AssimpConverter::traverseScene(
    const aiScene &scene, scene::Scene *outputScene, const aiNode &node,
    int parent)
{
    const uint32_t nodeIndex = outputScene->m_sceneGraph.addNode(parent);

    math::Transform lTrans;
    if (!convertTransform(&node.mTransformation, &lTrans))
        return false;
    const auto transIndex =
        static_cast<uint32_t>(outputScene->m_localTransforms.size());
    outputScene->m_localTransforms.emplace_back(std::move(lTrans));
    outputScene->m_globalTransforms.emplace_back();
    outputScene->m_nodeTransformIndexTable[nodeIndex] = transIndex;

    if (node.mNumMeshes > 0)
    {
        for (uint32_t i = 0; i < node.mNumMeshes; i++)
        {
            const uint32_t meshNodeIndex =
                outputScene->m_sceneGraph.addNode(static_cast<int>(nodeIndex));

            const uint32_t meshIndex = node.mMeshes[i];
            outputScene->m_nodeMeshIndexTable[meshNodeIndex] = meshIndex;
            outputScene->m_meshMaterialIndexTable[meshIndex] =
                scene.mMeshes[meshIndex]->mMaterialIndex;
        }
    }
    else
    {
        auto lightIter = std::find_if(
            outputScene->m_lights.begin(), outputScene->m_lights.end(),
            [&node](const scene::Light &light) {
                return node.mName.C_Str() == light.m_name;
            });
        if (lightIter != outputScene->m_lights.end())
        {
            const uint32_t lightNodeIndex =
                outputScene->m_sceneGraph.addNode(static_cast<int>(nodeIndex));

            const uint32_t lightIndex = static_cast<uint32_t>(
                lightIter - outputScene->m_lights.begin());
            outputScene->m_nodeLightIndexTable[lightNodeIndex] = lightIndex;
        }

        auto camIter = std::find_if(
            outputScene->m_cameras.begin(), outputScene->m_cameras.end(),
            [&node](const scene::Camera &camera) {
                return node.mName.C_Str() == camera.m_name;
            });
        if (camIter != outputScene->m_cameras.end())
        {
            const uint32_t camNodeIndex =
                outputScene->m_sceneGraph.addNode(static_cast<int>(nodeIndex));

            const uint32_t camIndex =
                static_cast<uint32_t>(camIter - outputScene->m_cameras.begin());
            outputScene->m_nodeCameraIndexTable[camNodeIndex] = camIndex;
        }
    }
    for (uint32_t i = 0; i < node.mNumChildren; i++)
        traverseScene(scene, outputScene, *node.mChildren[i], nodeIndex);

    return true;
}

bool kirana::scene::converters::AssimpConverter::convertScene(
    const void *inputScene, scene::Scene *outputScene)
{
    const aiScene &scene = *reinterpret_cast<const aiScene *>(inputScene);

    outputScene->m_name = scene.mName.C_Str();

    outputScene->m_meshes.resize(scene.mNumMeshes);
    outputScene->m_materials.resize(scene.mNumMaterials);
    outputScene->m_lights.resize(scene.mNumLights);
    outputScene->m_cameras.resize(scene.mNumCameras);

    for (uint32_t i = 0; i < scene.mNumMeshes; i++)
    {
        if (!convertMesh(scene.mMeshes[i], &outputScene->m_meshes[i]))
            return false;
    }

    for (uint32_t i = 0; i < scene.mNumMaterials; i++)
    {
        if (!convertMaterial(scene.mMaterials[i], &outputScene->m_materials[i]))
            return false;
    }

    for (uint32_t i = 0; i < scene.mNumLights; i++)
    {
        if (!convertLight(scene.mLights[i], &outputScene->m_lights[i]))
            return false;
    }

    for (uint32_t i = 0; i < scene.mNumCameras; i++)
    {
        if (!convertCamera(scene.mCameras[i], &outputScene->m_cameras[i]))
            return false;
    }

    return traverseScene(scene, outputScene, *scene.mRootNode, -1);
}
