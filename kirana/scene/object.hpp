#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <vector>
#include <memory>
#include <string>

#include <transform_hierarchy.hpp>
#include <bounds3.hpp>

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
    math::Bounds3 m_objectBounds;
    math::Bounds3 m_hierarchyBounds;
    std::unique_ptr<math::TransformHierarchy> m_transform = nullptr;

    static math::Matrix4x4 getMatrixFromNode(const aiNode *node);

    void m_addHierarchyBounds(const math::Bounds3 &bounds)
    {
        m_hierarchyBounds.encapsulate(bounds);
    }

  public:
    Object()
        : m_name{"Object"}, m_meshes{},
          m_transform{std::make_unique<math::TransformHierarchy>()}, m_objectBounds{},
          m_hierarchyBounds{} {};
    explicit Object(std::string name, std::shared_ptr<Mesh> mesh,
                    const math::TransformHierarchy &m_transform,
                    const math::Bounds3 &meshBounds);
    explicit Object(const aiNode *node,
                    std::vector<std::shared_ptr<Mesh>> meshes,
                    const math::Bounds3 &objectBounds,
                    math::TransformHierarchy *parent = nullptr);
    virtual ~Object() = default;

    Object(const Object &object) = delete;

    math::TransformHierarchy *const transform = m_transform.get();

    [[nodiscard]] inline const std::string &getName() const
    {
        return m_name;
    }
    [[nodiscard]] inline const std::vector<std::shared_ptr<Mesh>> &getMeshes()
        const
    {
        return m_meshes;
    }
    /// Returns the world-space bounding-box of the object. If the object has
    /// mesh objects, it'll encapsulate that. If it's empty, it'll just enclose
    /// the object's position.
    [[nodiscard]] inline math::Bounds3 getObjectBounds() const
    {
        return m_transform->transformBounds(m_objectBounds);
    }
    /// Returns the world-space bounding-box of the entire hierarchy of this
    /// object.
    [[nodiscard]] inline math::Bounds3 getHierarchyBounds() const
    {
        return m_transform->transformBounds(m_hierarchyBounds);
    }


    bool hasMesh(const Mesh *mesh) const;
};
} // namespace kirana::scene
#endif