#include "camera.hpp"

#include <vector2.hpp>
#include <ray.hpp>
#include <math_utils.hpp>

kirana::math::Vector3 kirana::scene::Camera::screenToWorldPosition(
    const math::Vector3 &screenPos, const math::Vector2 &resolution) const
{
    math::Vector4 pos(screenPos, 1.0f);
    pos[0] = (pos[0] / resolution[0]) * 2.0f - 1.0f;
    pos[1] = (pos[1] / resolution[1]) * 2.0f - 1.0f;
    pos[2] = math::clampf(pos[2], 0.0f, 1.0f);

    pos = m_inverseViewProjMatrix * pos;
    pos /= pos[3];
    return static_cast<math::Vector3>(pos);
}

kirana::math::Vector3 kirana::scene::Camera::worldToScreenPosition(
    const math::Vector3 &worldPos, const math::Vector2 &resolution) const
{
    math::Vector4 pos = m_viewProjMatrix * math::Vector4(worldPos, 1.0f);

    pos /= pos[3];

    pos[0] = (pos[0] * 0.5f + 0.5f) * resolution[0];
    pos[1] = (pos[1] * 0.5f + 0.5f) * resolution[1];
    pos[2] = math::map(pos[2], 0.0f, 1.0f, m_properties.nearPlane,
                       m_properties.farPlane);
    return static_cast<math::Vector3>(pos);
}


kirana::math::Ray kirana::scene::Camera::screenPositionToRay(
    const math::Vector2 &screenPos, const math::Vector2 &resolution) const
{
    const math::Vector3 origin{m_position};
    math::Vector3 direction{
        screenToWorldPosition(math::Vector3(screenPos[0], screenPos[1], 1.0f),
                              resolution) -
        origin};
    direction.normalize();
    return {origin, direction};
}