#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <vector>
#include <memory>
#include <string>

#include <transform.hpp>

struct aiNode;

namespace kirana::scene
{
class Mesh;
class Object
{
  private:
    std::string m_name;
    std::vector<std::shared_ptr<Mesh>> m_meshes;
    std::shared_ptr<Object> m_parent;
    math::Transform m_localTransform;
    math::Transform m_globalTransform;

    math::Matrix4x4 getMatrixFromNode(const aiNode *node) const;

  public:
    Object(const aiNode *node, const std::vector<std::shared_ptr<Mesh>> &meshes,
           std::shared_ptr<Object> parent);
    ~Object() = default;

    math::Transform *const localTransform = &m_localTransform;
    math::Transform *const globalTransform = &m_globalTransform;

    inline void setParent(const std::shared_ptr<Object> &p)
    {
        m_parent = p;
    }

    [[nodiscard]] inline const std::string &getName() const
    {
        return m_name;
    }
    [[nodiscard]] inline const std::vector<std::shared_ptr<Mesh>> &getMeshes()
        const
    {
        return m_meshes;
    }
    [[nodiscard]] inline const std::shared_ptr<Object> &getParent() const
    {
        return m_parent;
    }

    bool hasMesh(const Mesh *mesh) const;
};
} // namespace kirana::scene
#endif