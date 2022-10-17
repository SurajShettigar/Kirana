#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <transform.hpp>
#include <array>

namespace kirana::scene
{
using math::Matrix4x4;
using math::Transform;

class Camera
{
  protected:
    mutable std::array<uint32_t, 2> m_windowResolution;
    float m_nearPlane = 0.1f;
    float m_farPlane = 1000.0f;
    mutable float m_aspectRatio;

    Transform m_transform;
    mutable Transform m_projection;

  public:
    Camera(std::array<uint32_t, 2> windowResolution, float nearPlane = 0.1f,
           float farPlane = 1000.0f);
    ~Camera() = default;

    Camera(const Camera &camera);
    Camera &operator=(const Camera &camera);

    Transform &transform = m_transform;
    const Transform &projection = m_projection;

    const std::array<uint32_t, 2> &windowResolution = m_windowResolution;
    const float &nearPlane = m_nearPlane;
    const float &farPlane = m_farPlane;
    const float &aspectRatio = m_aspectRatio;

    virtual void setResolution(std::array<uint32_t, 2> resolution) const;
};
} // namespace kirana::scene

#endif