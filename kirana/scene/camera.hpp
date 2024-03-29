#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <transform_hierarchy.hpp>
#include <ray.hpp>
#include <array>
#include <event.hpp>

namespace kirana::scene
{
class SceneImporter;
class SceneManager;

using math::Matrix4x4;
using math::TransformHierarchy;

class Camera
{
    friend class SceneImporter;

  protected:
    mutable utils::Event<> m_onCameraChange;

    std::array<uint32_t, 2> m_windowResolution{1280, 720};
    float m_nearPlane = 0.1f;
    float m_farPlane = 1000.0f;
    float m_aspectRatio = 1.77778f;

    TransformHierarchy m_transform{nullptr};
    Matrix4x4 m_projection;

  public:
    Camera() = default;
    explicit Camera(std::array<uint32_t, 2> windowResolution,
                    float nearPlane = 0.1f, float farPlane = 1000.0f);
    virtual ~Camera() = default;

    Camera(const Camera &camera);
    Camera &operator=(const Camera &camera);

    inline const TransformHierarchy &getTransform() const
    {
        return m_transform;
    }

    inline void setTransform(const TransformHierarchy &transform)
    {
        m_transform = transform;
        m_onCameraChange();
    }

    const std::array<uint32_t, 2> &windowResolution = m_windowResolution;
    const float &nearPlane = m_nearPlane;
    const float &farPlane = m_farPlane;
    const float &aspectRatio = m_aspectRatio;

    inline uint32_t addOnCameraChangeEventListener(
        const std::function<void()> &callback) const
    {
        return m_onCameraChange.addListener(callback);
    }
    inline void removeOnCameraChangeEventListener(uint32_t callbackID) const
    {
        m_onCameraChange.removeListener(callbackID);
    }

    [[nodiscard]] inline Matrix4x4 getViewMatrix() const
    {
        return Matrix4x4::view(m_transform.getPosition(),
                               m_transform.getForward(), m_transform.getRight(),
                               m_transform.getUp());
    }

    // TODO: Get Inverse View Matrix

    [[nodiscard]] inline Matrix4x4 getProjectionMatrix() const
    {
        return m_projection;
    }
    // TODO: Get Inverse Projection Matrix

    [[nodiscard]] inline Matrix4x4 getViewProjectionMatrix() const
    {
        return getProjectionMatrix() * getViewMatrix();
    }

    // TODO: Get Inverse View-Projection Matrix

    void lookAt(const math::Vector3 &position,
                const math::Vector3 &up = math::Vector3::UP);

    [[nodiscard]] math::Vector3 screenToWorldPosition(
        const math::Vector3 &screenPos) const;
    [[nodiscard]] math::Vector3 worldToScreenPosition(
        const math::Vector3 &worldPos) const;
    [[nodiscard]] math::Ray screenPositionToRay(
        const math::Vector2 &screenPos) const;

    virtual void fitBoundsToView(const math::Vector3 &lookAtPosition,
                                 const math::Bounds3 &bounds,
                                 float distanceOffset = 0.0f) = 0;
    virtual void setResolution(std::array<uint32_t, 2> resolution);
};
} // namespace kirana::scene

#endif