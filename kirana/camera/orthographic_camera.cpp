#include "orthographic_camera.hpp"

kirana::camera::OrthographicCamera::OrthographicCamera(
    std::array<uint32_t, 2> windowResolution, float size, float nearPlane,
    float farPlane, bool graphicsAPI, bool flipY)
    : Camera{windowResolution, nearPlane, farPlane}, m_size{size},
      m_graphicsAPI{graphicsAPI}, m_flipY{flipY}
{
    m_projection = math::Transform::getOrthographicTransform(
        m_size, m_aspectRatio, m_nearPlane, m_farPlane, m_graphicsAPI, m_flipY);
}