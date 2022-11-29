#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <transform.hpp>
#include <array>
#include <event.hpp>

namespace kirana::scene
{
class SceneImporter;
class SceneManager;

using math::Matrix4x4;
using math::Transform;

class Camera
{
    friend class SceneImporter;

  protected:
    utils::Event<> m_onCameraChange;

    std::array<uint32_t, 2> m_windowResolution{1280, 720};
    float m_nearPlane = 0.1f;
    float m_farPlane = 1000.0f;
    float m_aspectRatio = 1.77778f;

    Transform m_transform{nullptr, true};
    Matrix4x4 m_projection;

    uint32_t m_transformChangeListener;
    void onTransformChanged();

  public:
    Camera() = default;
    explicit Camera(std::array<uint32_t, 2> windowResolution,
                    float nearPlane = 0.1f, float farPlane = 1000.0f);
    virtual ~Camera();

    Camera(const Camera &camera);
    Camera &operator=(const Camera &camera);

    Transform &transform = m_transform;

    const std::array<uint32_t, 2> &windowResolution = m_windowResolution;
    const float &nearPlane = m_nearPlane;
    const float &farPlane = m_farPlane;
    const float &aspectRatio = m_aspectRatio;

    inline uint32_t addOnCameraChangeEventListener(
        const std::function<void()> &callback)
    {
        return m_onCameraChange.addListener(callback);
    }
    inline void removeOnCameraChangeEventListener(uint32_t callbackID)
    {
        m_onCameraChange.removeListener(callbackID);
    }

    void lookAt(const math::Vector3 &position,
                const math::Vector3 &up = math::Vector3::UP);

    [[nodiscard]] inline Matrix4x4 getViewMatrix() const
    {
        return Matrix4x4::view(m_transform.getPosition(),
                               m_transform.getForward(), m_transform.getRight(),
                               m_transform.getUp());
    }

    [[nodiscard]] inline Matrix4x4 getProjectionMatrix() const
    {
        return m_projection;
    }

    [[nodiscard]] inline Matrix4x4 getViewProjectionMatrix() const
    {
        return getProjectionMatrix() * getViewMatrix();
    }

    virtual void setResolution(std::array<uint32_t, 2> resolution);
};
} // namespace kirana::scene

#endif