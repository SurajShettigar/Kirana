#ifndef KIRANA_SCENE_CAMERA_TYPES_HPP
#define KIRANA_SCENE_CAMERA_TYPES_HPP

namespace kirana::scene
{
enum class CameraProjection {
    PERSPECTIVE = 0,
    ORTHOGRAPHIC = 1,
};

struct CameraProperties {
    CameraProjection projection;
    float aspectRatio = 1.677777f;
    float nearPlane = 0.01f;
    float farPlane = 1000.0f;
    float orthoSize = 1.0f;
    float fieldOfView = 50.0f;
};
} // namespace kirana::scene

#endif