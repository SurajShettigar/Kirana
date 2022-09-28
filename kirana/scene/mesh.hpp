#ifndef MESH_HPP
#define MESH_HPP

#include <vector>

namespace kirana::scene
{

struct Vertex;

class Mesh
{
  private:
    std::vector<Vertex> m_vertices;
  public:
    Mesh() = default;
    ~Mesh() = default;
};
} // namespace kirana::scene
#endif