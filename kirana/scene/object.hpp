#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <vector>
#include <memory>

namespace kirana::scene
{

class Mesh;

class Object
{
  private:
    std::unique_ptr<Mesh> mesh;
    std::unique_ptr<Object> parent;
  public:
    Object() = default;
    ~Object() = default;
};
} // namespace kirana::scene
#endif