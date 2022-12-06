#include "perspective_camera.hpp"

#include <math_utils.hpp>

kirana::scene::PerspectiveCamera::PerspectiveCamera(
    std::array<uint32_t, 2> windowResolution, float fov, float nearPlane,
    float farPlane, bool graphicsAPI, bool flipY)
    : Camera{windowResolution, nearPlane, farPlane}, m_fov{fov},
      m_graphicsAPI{graphicsAPI}, m_flipY{flipY}
{
    m_projection = math::Matrix4x4::perspectiveProjection(
        m_fov, m_aspectRatio, m_nearPlane, m_farPlane, m_graphicsAPI, m_flipY);
}

kirana::scene::PerspectiveCamera::PerspectiveCamera(
    const PerspectiveCamera &camera)
    : Camera(camera)
{
    if (this != &camera)
    {
        m_fov = camera.m_fov;
        m_graphicsAPI = camera.m_graphicsAPI;
        m_flipY = camera.m_flipY;
    }
}
kirana::scene::PerspectiveCamera &kirana::scene::PerspectiveCamera::operator=(
    const PerspectiveCamera &camera)
{
    Camera::operator=(camera);
    if (this != &camera)
    {
        m_fov = camera.m_fov;
        m_graphicsAPI = camera.m_graphicsAPI;
        m_flipY = camera.m_flipY;
    }
    return *this;
}

void kirana::scene::PerspectiveCamera::fitBoundsToView(
    const math::Vector3 &lookAtPosition, const math::Bounds3 &bounds,
    const math::Vector3 &offset)
{
    const float extent = bounds.getExtent().length();
    const float distance = extent / std::sinf(math::radians(m_fov * 0.5f));
    m_transform.setPosition(math::Vector3::FORWARD * distance + offset);
    lookAt(lookAtPosition);
}

void kirana::scene::PerspectiveCamera::setResolution(
    std::array<uint32_t, 2> resolution)
{
    Camera::setResolution(resolution);
    m_projection = math::Matrix4x4::perspectiveProjection(
        m_fov, m_aspectRatio, m_nearPlane, m_farPlane, m_graphicsAPI, m_flipY);
    m_onCameraChange();
}