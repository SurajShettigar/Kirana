#include "mesh.hpp"
#include "scene_utils.hpp"

#include <assimp/scene.h>

kirana::scene::Mesh::Mesh(const aiMesh *mesh) : m_name{mesh->mName.C_Str()}
{
    m_vertices.resize(mesh->mNumVertices);
    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        if (mesh->HasPositions())
        {
            m_vertices[i].position.x = mesh->mVertices[i].x;
            m_vertices[i].position.y = mesh->mVertices[i].y;
            m_vertices[i].position.z = mesh->mVertices[i].z;
        }

        if (mesh->HasNormals())
        {
            m_vertices[i].normal.x = mesh->mNormals[i].x;
            m_vertices[i].normal.y = mesh->mNormals[i].y;
            m_vertices[i].normal.z = mesh->mNormals[i].z;
        }

        if (mesh->HasVertexColors(i))
        {
            m_vertices[i].color.r = mesh->mColors[i]->r;
            m_vertices[i].color.g = mesh->mColors[i]->g;
            m_vertices[i].color.b = mesh->mColors[i]->b;
        }
    }
}
