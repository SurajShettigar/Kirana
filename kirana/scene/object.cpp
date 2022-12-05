#include "object.hpp"
#include "mesh.hpp"
#include "scene_utils.hpp"

#include <assimp/scene.h>
#include <algorithm>
#include <utility>

using kirana::math::Matrix4x4;
using kirana::math::Transform;

Matrix4x4 kirana::scene::Object::getMatrixFromNode(const aiNode *node)
{
    const aiMatrix4x4 &mat = node->mTransformation;
    return Matrix4x4(mat.a1, mat.a2, mat.a3, mat.a4, mat.b1, mat.b2, mat.b3,
                     mat.b4, mat.c1, mat.c2, mat.c3, mat.c4, mat.d1, mat.d2,
                     mat.d3, mat.d4);
}


kirana::scene::Object::Object(std::string name, std::shared_ptr<Mesh> mesh,
                              const Transform &m_transform)
    : m_name{std::move(name)}, m_meshes{std::move(mesh)},
      m_transform{std::make_unique<Transform>(m_transform)},
      m_bounds{mesh->getBounds()}
{
}

kirana::scene::Object::Object(const aiNode *node,
                              std::vector<std::shared_ptr<Mesh>> meshes,
                              math::Transform *parent)
    : m_name{node->mName.C_Str()}, m_meshes{std::move(meshes)},
      m_transform{std::make_unique<Transform>(getMatrixFromNode(node), parent)}
{
    if (m_meshes.empty())
        m_bounds = math::Bounds3(m_transform->getPosition());
    else
    {
        m_bounds = math::Bounds3(m_meshes[0]->getBounds());
        for (size_t i = 1; i < m_meshes.size(); i++)
            m_bounds.encapsulate(m_meshes[i]->getBounds());
    }
}

bool kirana::scene::Object::hasMesh(const Mesh *const mesh) const
{
    auto it = std::find_if(
        m_meshes.begin(), m_meshes.end(),
        [&mesh](const std::shared_ptr<Mesh> &m) { return mesh == m.get(); });

    return it != m_meshes.end();
}