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

    const std::string &name = m_name;
    const std::vector<Vertex> &vertices = m_vertices;
};
} // namespace kirana::scene
#endif