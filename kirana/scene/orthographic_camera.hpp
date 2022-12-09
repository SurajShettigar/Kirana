#ifndef ORTHOGRAPHIC_CAMERA_HPP
#define ORTHOGRAPHIC_CAMERA_HPP

#include "camera.hpp"

namespace kirana::scene
{
class OrthographicCamera : public Camera
{
  protected:
    float m_size = 1.0f;
    bool m_graphicsAPI = false;
    bool m_flipY = false;

  public:
    explicit OrthographicCamera(const std::array<uint32_t, 2> &windowResolution,
                       float size = 1.0f, float nearPlane = 0.1f,
                       float farPlane = 1000.0f, bool graphicsAPI = false,
                       bool flipY = false);
    ~OrthographicCamera() override = default;

    OrthographicCamera(const OrthographicCamera &camera);
    OrthographicCamera &operator=(const OrthographicCamera &camera);

    const float &size = m_size;

    void fitBoundsToView(const math::Vector3 &lookAtPosition,
                         const math::Bounds3 &bounds,
                         float distanceOffset = 0.0f) override;
    void setResolution(std::array<uint32_t, 2> resolution) override;
};
} // namespace kirana::scene

#endif