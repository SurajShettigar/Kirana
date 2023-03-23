#ifndef KIRANA_SCENE_LIGHT_HPP
#define KIRANA_SCENE_LIGHT_HPP

#include "object.hpp"

namespace kirana::scene
{

class Light : public Object
{
    friend class external::AssimpSceneConverter;
  public:
    Light() = default;
    explicit Light(std::string name) : Object(std::move(name)){};
    ~Light() override = default;

    Light(const Light &light) = default;
    Light(Light &&light) = default;
    Light &operator=(const Light &light) = default;
    Light &operator=(Light &&light) = default;

  protected:
};

} // namespace kirana::scene

#endif