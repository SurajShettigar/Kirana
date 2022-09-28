#ifndef MESH_HPP
#define MESH_HPP

#include <vector>
#include <memory>

namespace kirana::scene
{
class Object;

class Scene
{
  private:
    std::vector<std::unique_ptr<Object>> m_objects;
  public:
    Scene() = default;
    ~Scene() = default;
};
} // namespace kirana::scene
#endif