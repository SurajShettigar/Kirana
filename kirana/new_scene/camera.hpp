#ifndef KIRANA_SCENE_CAMERA_HPP
#define KIRANA_SCENE_CAMERA_HPP

#include "object.hpp"
#include "camera_types.hpp"

#include <transform.hpp>

namespace kirana::scene
{

class Camera : public Object
{
    friend class external::AssimpSceneConverter;

  public:
    static const Camera DEFAULT_PERSPECTIVE_CAM;

    Camera() = default;
    explicit Camera(std::string name, CameraProperties properties,
                    const math::Transform &globalTransform)
        : Object(std::move(name)), m_properties{properties},
          m_projectionMatrix{calculateProjectionMatrix(m_properties)},
          m_viewMatrix{calculateViewMatrix(globalTransform)},
          m_viewProjMatrix{m_projectionMatrix * m_viewMatrix},
          m_inverseViewProjMatrix{math::Matrix4x4::inverse(m_viewProjMatrix)},
          m_position{globalTransform.getPosition()},
          m_forward{globalTransform.getForward()} {};
    ~Camera() override = default;

    Camera(const Camera &camera) = default;
    Camera(Camera &&camera) = default;
    Camera &operator=(const Camera &camera) = default;
    Camera &operator=(Camera &&camera) = default;

    [[nodiscard]] inline const CameraProperties &getProperties() const
    {
        return m_properties;
    }
    [[nodiscard]] inline const math::Matrix4x4 &getProjectionMatrix() const
    {
        return m_projectionMatrix;
    }
    [[nodiscard]] inline const math::Matrix4x4 &getViewMatrix() const
    {
        return m_viewMatrix;
    }
    [[nodiscard]] inline const math::Matrix4x4 &getViewProjectionMatrix() const
    {
        return m_viewProjMatrix;
    }
    [[nodiscard]] inline const math::Matrix4x4 &getInverseViewProjectionMatrix()
        const
    {
        return m_inverseViewProjMatrix;
    }

    void setProperties(const CameraProperties &properties)
    {
        m_properties = properties;
        m_projectionMatrix = calculateProjectionMatrix(m_properties);
        m_viewProjMatrix = m_projectionMatrix * m_viewMatrix;
        m_inverseViewProjMatrix = math::Matrix4x4::inverse(m_viewProjMatrix);
    }

    void setTransform(const math::Transform &globalTransform)
    {
        m_viewMatrix = calculateViewMatrix(globalTransform);
        m_viewProjMatrix = m_projectionMatrix * m_viewMatrix;
        m_inverseViewProjMatrix = math::Matrix4x4::inverse(m_viewProjMatrix);
        m_position = globalTransform.getPosition();
        m_forward = globalTransform.getForward();
    }

    [[nodiscard]] math::Vector3 screenToWorldPosition(
        const math::Vector3 &screenPos, const math::Vector2 &resolution) const;
    [[nodiscard]] math::Vector3 worldToScreenPosition(
        const math::Vector3 &worldPos, const math::Vector2 &resolution) const;
    [[nodiscard]] math::Ray screenPositionToRay(
        const math::Vector2 &screenPos, const math::Vector2 &resolution) const;

  protected:
    CameraProperties m_properties;
    math::Matrix4x4 m_projectionMatrix;
    math::Matrix4x4 m_viewMatrix;
    math::Matrix4x4 m_viewProjMatrix;
    math::Matrix4x4 m_inverseViewProjMatrix;
    math::Vector3 m_position;
    math::Vector3 m_forward;

    inline static math::Matrix4x4 calculateProjectionMatrix(
        const CameraProperties &properties)
    {
        return properties.projection == CameraProjection::ORTHOGRAPHIC
            ? math::Matrix4x4::orthographicProjection(
                  properties.orthoSize, properties.aspectRatio,
                  properties.nearPlane, properties.farPlane, true, true)
            : math::Matrix4x4::perspectiveProjection(
                  properties.fieldOfView, properties.aspectRatio,
                  properties.nearPlane, properties.farPlane, true, true);
    }

    inline static math::Matrix4x4 calculateViewMatrix(
        const math::Transform &globalTransform)
    {
        return math::Matrix4x4::view(
            globalTransform.getPosition(), globalTransform.getForward(),
            globalTransform.getRight(), globalTransform.getUp());
    }
};

} // namespace kirana::scene

#endif