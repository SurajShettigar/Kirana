#ifndef KIRANA_SCENE_CAMERA_HPP
#define KIRANA_SCENE_CAMERA_HPP

#include "object.hpp"

namespace kirana::scene
{

class Camera : public Object
{
    friend class converters::AssimpConverter;
  public:
    Camera() = default;
    explicit Camera(std::string name) : Object(std::move(name)){};
    virtual ~Camera() override = default;

    Camera(const Camera &camera) = default;
    Camera(Camera &&camera) = default;
    Camera &operator=(const Camera &camera) = default;
    Camera &operator=(Camera &&camera) = default;

  protected:
};

} // namespace kirana::scene

#endif