#ifndef MESH_HPP
#define MESH_HPP

#include <vector>
#include <string>
#include <memory>

struct aiMesh;

namespace kirana::scene
{
struct Vertex;
class Material;
class Mesh
{
  protected:
    std::string m_name;
    std::vector<Vertex> m_vertices;
    std::vector<uint32_t> m_indices;
    std::shared_ptr<Material> m_material;

  public:
    Mesh() = default;
    Mesh(std::string name, std::vector<Vertex> vertices,
         std::vector<uint32_t> indices,
         std::shared_ptr<Material> material);
    Mesh(const aiMesh *mesh, std::shared_ptr<Material> material);
    virtual ~Mesh() = default;

    [[nodiscard]] inline const std::string &getName() const
    {
        return m_name;
    }
    [[nodiscard]] inline const std::vector<Vertex> &getVertices() const
    {
        return m_vertices;
    }
    [[nodiscard]] inline const std::vector<uint32_t> &getIndices() const
    {
        return m_indices;
    }
    [[nodiscard]] inline const std::shared_ptr<Material> &getMaterial() const
    {
        return m_material;
    }
};
} // namespace kirana::scene
#endif