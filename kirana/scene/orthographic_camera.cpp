#include "orthographic_camera.hpp"

kirana::scene::OrthographicCamera::OrthographicCamera(
    const std::array<uint32_t, 2> &windowResolution, float size,
    float nearPlane, float farPlane, bool graphicsAPI, bool flipY)
    : Camera{windowResolution, nearPlane, farPlane}, m_size{size},
      m_graphicsAPI{graphicsAPI}, m_flipY{flipY}
{
    m_projection = math::Matrix4x4::orthographicProjection(
        m_size, m_aspectRatio, m_nearPlane, m_farPlane, m_graphicsAPI, m_flipY);
}

kirana::scene::OrthographicCamera::OrthographicCamera(
    const OrthographicCamera &camera)
    : Camera(camera)
{
    if (this != &camera)
    {
        m_size = camera.m_size;
        m_graphicsAPI = camera.m_graphicsAPI;
        m_flipY = camera.m_flipY;
    }
}

kirana::scene::OrthographicCamera &kirana::scene::OrthographicCamera::operator=(
    const OrthographicCamera &camera)
{
    Camera::operator=(camera);
    if (this != &camera)
    {
        m_size = camera.m_size;
        m_graphicsAPI = camera.m_graphicsAPI;
        m_flipY = camera.m_flipY;
    }
    return *this;
}

void kirana::scene::OrthographicCamera::fitBoundsToView(
    const math::Vector3 &lookAtPosition, const math::Bounds3 &bounds,
    float distanceOffset)
{
    // TODO: Fit bounds to view for orthographic camera.
}

void kirana::scene::OrthographicCamera::setResolution(
    std::array<uint32_t, 2> resolution)
{
    Camera::setResolution(resolution);
    m_projection = math::Matrix4x4::orthographicProjection(
        m_size, m_aspectRatio, m_nearPlane, m_farPlane, m_graphicsAPI, m_flipY);
    m_onCameraChange();
}