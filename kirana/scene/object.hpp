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

  protected:
    std::string m_name;
    std::vector<std::shared_ptr<Mesh>> m_meshes;
    std::unique_ptr<math::Transform> m_transform = nullptr;

    math::Matrix4x4 getMatrixFromNode(const aiNode *node) const;

  public:
    Object()
        : m_name{"Object"}, m_meshes{},
          m_transform{std::make_unique<math::Transform>()} {};
    explicit Object(std::string name, std::shared_ptr<Mesh> mesh,
                    const math::Transform &m_transform);
    explicit Object(const aiNode *node,
                    std::vector<std::shared_ptr<Mesh>> meshes,
                    math::Transform *parent = nullptr);
    virtual ~Object() = default;

    math::Transform *const transform = m_transform.get();

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