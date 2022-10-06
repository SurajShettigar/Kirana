#ifndef PERSPECTIVE_CAMERA_HPP
#define PERSPECTIVE_CAMERA_HPP

#include "camera.hpp"

namespace kirana::camera
{
using math::Matrix4x4;
using math::Transform;

class PerspectiveCamera : public Camera
{
  protected:
    float m_fov = 60.0f;
    bool m_flipY = false;
  public:
    PerspectiveCamera(std::array<uint32_t, 2> windowResolution,
                      float fov = 60.0f, float nearPlane = 0.1f,
                      float farPlane = 1000.0f, bool flipY = false);
    ~PerspectiveCamera() = default;

    const float &fieldOfView = m_fov;
};
} // namespace kirana::camera

#endif