#ifndef PLANE_HPP
#define PLANE_HPP

#include "../object.hpp"

namespace kirana::scene {
class Material;
}
namespace kirana::scene::primitives
{
class Plane : public Object
{
  public:
    explicit Plane(const std::shared_ptr<Material> &material);
    ~Plane() override = default;
};
} // namespace kirana::scene::primitives
#endif