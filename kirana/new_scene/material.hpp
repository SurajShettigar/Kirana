#ifndef KIRANA_SCENE_MATERIAL_HPP
#define KIRANA_SCENE_MATERIAL_HPP

#include "object.hpp"
namespace kirana::scene
{
class Material : public Object
{
    friend class converters::AssimpConverter;
  public:
    Material() = default;
    explicit Material(std::string name) : Object(std::move(name)){};
    ~Material() override = default;

    Material(const Material &mesh) = default;
    Material(Material &&mesh) = default;
    Material &operator=(const Material &mesh) = default;
    Material &operator=(Material &&mesh) = default;

  protected:
};

} // namespace kirana::scene

#endif