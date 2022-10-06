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
            m_vertices[i].position[0] = mesh->mVertices[i].x;
            m_vertices[i].position[1] = mesh->mVertices[i].y;
            m_vertices[i].position[2] = mesh->mVertices[i].z;
        }

        if (mesh->HasNormals())
        {
            m_vertices[i].normal[0] = mesh->mNormals[i].x;
            m_vertices[i].normal[1] = mesh->mNormals[i].y;
            m_vertices[i].normal[2] = mesh->mNormals[i].z;
        }

        if (mesh->HasVertexColors(i))
        {
            m_vertices[i].color[0] = mesh->mColors[i]->r;
            m_vertices[i].color[1] = mesh->mColors[i]->g;
            m_vertices[i].color[2] = mesh->mColors[i]->b;
        }
    }
}
