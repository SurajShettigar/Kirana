#ifndef PERSPECTIVE_CAMERA_HPP
#define PERSPECTIVE_CAMERA_HPP

#include "camera.hpp"

namespace kirana::camera
{
using math::Matrix4x4;
using math::Transform;

class OrthographicCamera : public Camera
{
  protected:
    float m_size = 1.0f;

    Transform m_projection;
  public:
    OrthographicCamera(std::array<uint32_t, 2> windowResolution,
                      float size = 1.0f, float nearPlane = 0.1f,
                      float farPlane = 1000.0f);
    ~OrthographicCamera() = default;

    const float &size = m_size;
};
} // namespace kirana::camera

#endif