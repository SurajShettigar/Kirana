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
class Scene;
class Object
{
    friend class Scene;

  private:
    std::string m_name;
    std::vector<std::shared_ptr<Mesh>> m_meshes;
    math::Transform *m_transform = nullptr;

    math::Matrix4x4 getMatrixFromNode(const aiNode *node) const;

  public:
    Object(const aiNode *node, const std::vector<std::shared_ptr<Mesh>> &meshes,
           math::Transform *parent = nullptr);
    ~Object();

    math::Transform *const transform = m_transform;

    [[nodiscard]] inline const std::string &getName() const
    {
        return m_name;
    }
    [[nodiscard]] inline const std::vector<std::shared_ptr<Mesh>> &getMeshes()
        const
    {
        return m_meshes;
    }

    bool hasMesh(const Mesh *mesh) const;
};
} // namespace kirana::scene
#endif