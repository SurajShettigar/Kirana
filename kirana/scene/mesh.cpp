#include "mesh.hpp"
#include "scene_utils.hpp"
#include "material.hpp"

#include <assimp/scene.h>
#include <constants.h>
#include <logger.hpp>
#include <utility>

typedef kirana::utils::Logger Logger;
typedef kirana::utils::LogSeverity LogSeverity;
namespace constants = kirana::utils::constants;

kirana::scene::Mesh::Mesh(const std::string &name, const math::Bounds3 &bounds,
                          const std::vector<Vertex> &vertices,
                          const std::vector<uint32_t> &indices,
                          const std::shared_ptr<Material> &material)
    : m_name{name}, m_bounds{bounds}, m_vertices{vertices}, m_indices{indices},
      m_material{material}
{
}

kirana::scene::Mesh::Mesh(const aiMesh *mesh,
                          std::shared_ptr<Material> material)
    : m_name{mesh->mName.C_Str()}, m_bounds{math::Vector3{mesh->mAABB.mMin.x,
                                                          mesh->mAABB.mMin.y,
                                                          mesh->mAABB.mMin.z},
                                            math::Vector3{mesh->mAABB.mMax.x,
                                                          mesh->mAABB.mMax.y,
                                                          mesh->mAABB.mMax.z}},
      m_material{std::move(material)}
{
    if ((mesh->mPrimitiveTypes & aiPrimitiveType::aiPrimitiveType_TRIANGLE) ==
        aiPrimitiveType::aiPrimitiveType_TRIANGLE)
    {
        m_vertices.resize(mesh->mNumVertices);
        for (size_t i = 0; i < mesh->mNumFaces; i++)
        {
            const aiFace &face = mesh->mFaces[i];
            const uint32_t v1 = face.mIndices[0];
            const uint32_t v2 = face.mIndices[1];
            const uint32_t v3 = face.mIndices[2];
            m_indices.push_back(v1);
            m_indices.push_back(v2);
            m_indices.push_back(v3);

            m_vertices[v1].position[0] = mesh->mVertices[v1].x;
            m_vertices[v1].position[1] = mesh->mVertices[v1].y;
            m_vertices[v1].position[2] = mesh->mVertices[v1].z;

            m_vertices[v2].position[0] = mesh->mVertices[v2].x;
            m_vertices[v2].position[1] = mesh->mVertices[v2].y;
            m_vertices[v2].position[2] = mesh->mVertices[v2].z;

            m_vertices[v3].position[0] = mesh->mVertices[v3].x;
            m_vertices[v3].position[1] = mesh->mVertices[v3].y;
            m_vertices[v3].position[2] = mesh->mVertices[v3].z;

            if (mesh->HasNormals())
            {
                m_vertices[v1].normal[0] = mesh->mNormals[v1].x;
                m_vertices[v1].normal[1] = mesh->mNormals[v1].y;
                m_vertices[v1].normal[2] = mesh->mNormals[v1].z;

                m_vertices[v2].normal[0] = mesh->mNormals[v2].x;
                m_vertices[v2].normal[1] = mesh->mNormals[v2].y;
                m_vertices[v2].normal[2] = mesh->mNormals[v2].z;

                m_vertices[v3].normal[0] = mesh->mNormals[v3].x;
                m_vertices[v3].normal[1] = mesh->mNormals[v3].y;
                m_vertices[v3].normal[2] = mesh->mNormals[v3].z;
            }

            if (mesh->HasVertexColors(v1))
            {
                m_vertices[v1].color[0] = mesh->mColors[v1]->r;
                m_vertices[v1].color[1] = mesh->mColors[v1]->g;
                m_vertices[v1].color[2] = mesh->mColors[v1]->b;
            }
            if (mesh->HasVertexColors(v2))
            {
                m_vertices[v2].color[0] = mesh->mColors[v2]->r;
                m_vertices[v2].color[1] = mesh->mColors[v2]->g;
                m_vertices[v2].color[2] = mesh->mColors[v2]->b;
            }
            if (mesh->HasVertexColors(v3))
            {
                m_vertices[v3].color[0] = mesh->mColors[v3]->r;
                m_vertices[v3].color[1] = mesh->mColors[v3]->g;
                m_vertices[v3].color[2] = mesh->mColors[v3]->b;
            }
        }
    }
    else
    {
        Logger::get().log(constants::LOG_CHANNEL_SCENE, LogSeverity::error,
                          "Mesh: " + m_name + " does not have triangle faces");
    }
}
