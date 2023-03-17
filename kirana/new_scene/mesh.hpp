#ifndef KIRANA_SCENE_MESH_HPP
#define KIRANA_SCENE_MESH_HPP

#include "object.hpp"
#include "scene_types.hpp"

namespace kirana::scene
{
class Mesh : public Object
{
    friend class converters::AssimpConverter;

  public:
    Mesh() = default;
    explicit Mesh(std::string name) : Object(std::move(name)){};
    explicit Mesh(std::string name, std::vector<Vertex> vertices,
                  std::vector<INDEX_TYPE> indices,
                  const math::Bounds3 &boundingBox)
        : Object(std::move(name)), m_vertices{std::move(vertices)},
          m_indices{std::move(indices)}, m_bounds{boundingBox}
    {
    }
    ~Mesh() override = default;

    Mesh(const Mesh &mesh) = default;
    Mesh(Mesh &&mesh) = default;
    Mesh &operator=(const Mesh &mesh) = default;
    Mesh &operator=(Mesh &&mesh) = default;

    [[nodiscard]] inline const std::vector<Vertex> &getVertices() const
    {
        return m_vertices;
    }

    [[nodiscard]] inline const std::vector<INDEX_TYPE> &getIndices() const
    {
        return m_indices;
    }

    [[nodiscard]] inline const math::Bounds3 &getBounds() const
    {
        return m_bounds;
    }

  protected:
    std::vector<Vertex> m_vertices;
    std::vector<INDEX_TYPE> m_indices;
    math::Bounds3 m_bounds;
};

} // namespace kirana::scene

#endif