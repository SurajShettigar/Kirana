#include "camera.hpp"


kirana::camera::Camera::Camera(std::array<uint32_t, 2> windowResolution,
                               float nearPlane, float farPlane)
    : m_windowResolution{windowResolution}, m_nearPlane{nearPlane},
      m_farPlane{farPlane}, m_aspectRatio{
                                static_cast<float>(m_windowResolution[0]) /
                                static_cast<float>(m_windowResolution[1])}
{
}