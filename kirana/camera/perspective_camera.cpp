#include "perspective_camera.hpp"

kirana::camera::PerspectiveCamera::PerspectiveCamera(
    std::array<uint32_t, 2> windowResolution, float fov, float nearPlane,
    float farPlane, bool graphicsAPI, bool flipY)
    : Camera{windowResolution, nearPlane, farPlane}, m_fov{fov},
      m_graphicsAPI{graphicsAPI}, m_flipY{flipY}
{
    m_projection = math::Transform::getPerspectiveTransform(
        fov, m_aspectRatio, m_nearPlane, m_farPlane, m_graphicsAPI, m_flipY);
}