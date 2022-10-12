#ifndef MESH_HPP
#define MESH_HPP

#include <vector>
#include <string>

struct aiMesh;

namespace kirana::scene
{
struct Vertex;
class Mesh
{
  private:
    std::string m_name;
    std::vector<Vertex> m_vertices;

  public:
    Mesh(const aiMesh *mesh);
    ~Mesh() = default;

    [[nodiscard]] inline const std::string &getName() const
    {
        return m_name;
    }
    [[nodiscard]] inline const std::vector<Vertex> &getVertices() const
    {
        return m_vertices;
    }
};
} // namespace kirana::scene
#endif