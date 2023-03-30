#include "object.hpp"
#include "mesh.hpp"
#include "scene_types.hpp"

#include <assimp/scene.h>
#include <algorithm>
#include <utility>

using kirana::math::Matrix4x4;
using kirana::math::TransformHierarchy;

Matrix4x4 kirana::scene::Object::getMatrixFromNode(const aiNode *node)
{
    const aiMatrix4x4 &mat = node->mTransformation;
    return Matrix4x4(mat.a1, mat.a2, mat.a3, mat.a4, mat.b1, mat.b2, mat.b3,
                     mat.b4, mat.c1, mat.c2, mat.c3, mat.c4, mat.d1, mat.d2,
                     mat.d3, mat.d4);
}


kirana::scene::Object::Object(std::string name, std::shared_ptr<Mesh> mesh,
                              const TransformHierarchy &m_transform,
                              const math::Bounds3 &meshBounds)
    : m_name{std::move(name)}, m_meshes{std::move(mesh)},
      m_transform{std::make_unique<TransformHierarchy>(m_transform)},
      m_objectBounds{meshBounds}, m_hierarchyBounds{m_objectBounds}
{
}

kirana::scene::Object::Object(const aiNode *node,
                              std::vector<std::shared_ptr<Mesh>> meshes,
                              const math::Bounds3 &objectBounds,
                              math::TransformHierarchy *parent)
    : m_name{node->mName.C_Str()}, m_meshes{std::move(meshes)},
      m_objectBounds{objectBounds}, m_hierarchyBounds{m_objectBounds},
      m_transform{std::make_unique<TransformHierarchy>(getMatrixFromNode(node), parent)}
{
}

bool kirana::scene::Object::hasMesh(const Mesh *const mesh) const
{
    auto it = std::find_if(
        m_meshes.begin(), m_meshes.end(),
        [&mesh](const std::shared_ptr<Mesh> &m) { return mesh == m.get(); });

    return it != m_meshes.end();
}