#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <transform.hpp>
#include <array>

namespace kirana::camera
{
using math::Matrix4x4;
using math::Transform;

class Camera
{
  protected:
    std::array<uint32_t, 2> m_windowResolution;
    float m_nearPlane = 0.1f;
    float m_farPlane = 1000.0f;
    float m_aspectRatio;

    Transform m_transform;
    Transform m_projection;
  public:
    Camera(std::array<uint32_t, 2> windowResolution,
           float nearPlane = 0.1f, float farPlane = 1000.0f);
    ~Camera() = default;

    Transform &transform = m_transform;
    const Transform &projection = m_projection;

    const std::array<uint32_t, 2> &windowResolution = m_windowResolution;
    const float &nearPlane = m_nearPlane;
    const float &farPlane = m_farPlane;
    const float &aspectRatio = m_aspectRatio;
};
} // namespace kirana::camera

#endif