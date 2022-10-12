#ifndef SCENE_UTILS_HPP
#define SCENE_UTILS_HPP

#include <vector3.hpp>
#include <perspective_camera.hpp>

namespace kirana::scene
{
struct Vertex
{
    math::Vector3 position;
    math::Vector3 normal;
    math::Vector3 color;
};

struct SceneImportSettings
{
    bool calculateTangentSpace = false;
    bool joinIdenticalVertices = false;
    bool triangulate = true;
    bool generateNormals = false;
    bool generateSmoothNormals = false;
    bool improveCacheLocality = true;
    bool optimizeMesh = true;
    bool preTransformVertices = false;
};

static const SceneImportSettings DEFAULT_SCENE_IMPORT_SETTINGS{
    false, false, false, false, false, true, true, false};

static const camera::PerspectiveCamera DEFAULT_CAMERA{{1280, 720}, 60.0f, 0.1f,
                                                      1000.0f,     true,  true};

} // namespace kirana::scene

#endif