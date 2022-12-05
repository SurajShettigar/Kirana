#include "camera.hpp"

#include <math_utils.hpp>


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
    m_transform.lookAt(m_transform.getPosition() - position, up);
    m_onCameraChange();
}

kirana::math::Vector3 kirana::scene::Camera::screenToWorldPosition(
    const math::Vector3 &screenPos) const
{
    math::Vector4 pos(screenPos, 1.0f);
    pos[0] = (pos[0] / static_cast<float>(m_windowResolution[0])) * 2.0f - 1.0f;
    pos[1] = (pos[1] / static_cast<float>(m_windowResolution[1])) * 2.0f - 1.0f;
    pos[2] = math::clampf(pos[2], 0.0f, 1.0f);

    pos = Matrix4x4::inverse(getViewProjectionMatrix()) * pos;
    pos /= pos[3];
    return static_cast<math::Vector3>(pos);
}

kirana::math::Vector3 kirana::scene::Camera::worldToScreenPosition(
    const math::Vector3 &worldPos) const
{
    math::Vector4 pos =
        getViewProjectionMatrix() * math::Vector4(worldPos, 1.0f);

    pos /= pos[3];

    pos[0] = (pos[0] * 0.5f + 0.5f) * static_cast<float>(m_windowResolution[0]);
    pos[1] = (pos[1] * 0.5f + 0.5f) * static_cast<float>(m_windowResolution[1]);
    pos[2] = math::map(pos[2], 0.0f, 1.0f, m_nearPlane, m_farPlane);
    return static_cast<math::Vector3>(pos);
}


kirana::math::Ray kirana::scene::Camera::screenPositionToRay(
    const math::Vector3 &screenPos) const
{
    return {m_transform.getPosition(),
            math::Vector3::normalize(screenToWorldPosition(screenPos))};
}

void kirana::scene::Camera::setResolution(std::array<uint32_t, 2> resolution)
{
    m_windowResolution = resolution;
    m_aspectRatio = static_cast<float>(m_windowResolution[0]) /
                    static_cast<float>(m_windowResolution[1]);
}