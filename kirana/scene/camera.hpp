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
    friend class SceneManager;

  protected:
    utils::Event<> m_onCameraChange;

    std::array<uint32_t, 2> m_windowResolution;
    float m_nearPlane = 0.1f;
    float m_farPlane = 1000.0f;
    float m_aspectRatio;

    Transform m_transform;
    Matrix4x4 m_projection;

  public:
    Camera(std::array<uint32_t, 2> windowResolution, float nearPlane = 0.1f,
           float farPlane = 1000.0f);
    ~Camera() = default;

    Camera(const Camera &camera);
    Camera &operator=(const Camera &camera);

    // TODO: Call on camera change event when transform changes.
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

    [[nodiscard]] inline Matrix4x4 getViewMatrix() const
    {
        return Matrix4x4::inverse(m_transform.getMatrix());
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