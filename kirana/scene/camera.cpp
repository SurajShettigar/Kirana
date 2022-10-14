#include "camera.hpp"


kirana::scene::Camera::Camera(std::array<uint32_t, 2> windowResolution,
                              float nearPlane, float farPlane)
    : m_windowResolution{windowResolution}, m_nearPlane{nearPlane},
      m_farPlane{farPlane}, m_aspectRatio{
                                static_cast<float>(m_windowResolution[0]) /
                                static_cast<float>(m_windowResolution[1])}
{
}

kirana::scene::Camera::Camera(const Camera &camera)
{
    if (this != &camera)
    {
        m_windowResolution = camera.m_windowResolution;
        m_nearPlane = camera.m_nearPlane;
        m_farPlane = camera.m_farPlane;
        m_aspectRatio = camera.m_aspectRatio;
        m_transform = camera.m_transform;
        m_projection = camera.m_projection;
    }
}
kirana::scene::Camera &kirana::scene::Camera::operator=(const Camera &camera)
{
    if (this != &camera)
    {
        m_windowResolution = camera.m_windowResolution;
        m_nearPlane = camera.m_nearPlane;
        m_farPlane = camera.m_farPlane;
        m_aspectRatio = camera.m_aspectRatio;
        m_transform = camera.m_transform;
        m_projection = camera.m_projection;
    }
    return *this;
}