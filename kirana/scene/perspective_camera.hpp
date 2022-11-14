#ifndef PERSPECTIVE_CAMERA_HPP
#define PERSPECTIVE_CAMERA_HPP

#include "camera.hpp"

namespace kirana::scene
{
using math::Matrix4x4;
using math::Transform;

class PerspectiveCamera : public Camera
{
  protected:
    float m_fov = 60.0f;
    bool m_graphicsAPI = false;
    bool m_flipY = false;

  public:
    PerspectiveCamera(std::array<uint32_t, 2> windowResolution,
                      float fov = 60.0f, float nearPlane = 0.1f,
                      float farPlane = 1000.0f, bool graphicsAPI = false,
                      bool flipY = false);
    ~PerspectiveCamera() = default;

    PerspectiveCamera(const PerspectiveCamera &camera);
    PerspectiveCamera &operator=(const PerspectiveCamera &camera);

    const float &fieldOfView = m_fov;

    void setResolution(std::array<uint32_t, 2> resolution) override;
};
} // namespace kirana::scene

#endif