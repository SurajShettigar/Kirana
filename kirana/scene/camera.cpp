#include "camera.hpp"

#include "vector3.hpp"


void kirana::scene::Camera::onTransformChanged()
{
    m_onCameraChange();
}

kirana::scene::Camera::Camera(std::array<uint32_t, 2> windowResolution,
                              float nearPlane, float farPlane)
    : m_windowResolution{windowResolution}, m_nearPlane{nearPlane},
      m_farPlane{farPlane},
      m_aspectRatio{static_cast<float>(m_windowResolution[0]) /
                    static_cast<float>(m_windowResolution[1])},
      m_transform{nullptr, true}
{
    m_transformChangeListener =
        m_transform.addOnChangeListener([&]() { onTransformChanged(); });
}

kirana::scene::Camera::~Camera()
{
    m_transform.removeOnChangeListener(m_transformChangeListener);
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
        m_transformChangeListener =
            m_transform.addOnChangeListener([&]() { onTransformChanged(); });
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
        m_transformChangeListener =
            m_transform.addOnChangeListener([&]() { onTransformChanged(); });
    }
    return *this;
}

void kirana::scene::Camera::lookAt(const math::Vector3 &position,
                                   const math::Vector3 &up)
{
    m_transform.lookAt(m_transform.getPosition() - position, math::Vector3::UP);
    m_onCameraChange();
}

void kirana::scene::Camera::setResolution(std::array<uint32_t, 2> resolution)
{
    m_windowResolution = resolution;
    m_aspectRatio = static_cast<float>(m_windowResolution[0]) /
                    static_cast<float>(m_windowResolution[1]);
}