#ifndef SCENE_UTILS_HPP
#define SCENE_UTILS_HPP

#include <vector3.hpp>

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
    bool optimizeMesh = false;
    bool preTransformVertices = false;
};

static const SceneImportSettings DEFAULT_SCENE_IMPORT_SETTINGS{};

} // namespace kirana::scene

#endif