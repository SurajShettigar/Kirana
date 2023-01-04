#ifndef SCENE_UTILS_HPP
#define SCENE_UTILS_HPP

#include <transform.hpp>

namespace kirana::scene
{
class Object;
class Material;
struct Vertex
{
    math::Vector3 position;
    alignas(16) math::Vector3 normal;
    alignas(16) math::Vector3 color;
};

struct WorldData
{
    math::Vector4 ambientColor{0.5f, 0.5f, 0.5f, 1.0f};
    math::Vector3 sunDirection{0.25f, -0.75f, -0.25f};
    alignas(4) float sunIntensity{2.0f};
    alignas(16) math::Vector4 sunColor{1.0f, 1.0f, 1.0f, 1.0f};
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
    bool generateBoundingBoxes = true;
};

struct Renderable
{
    const Object *object = nullptr;
    bool overrideMaterial = false;
    bool selected = false;
    bool renderVisible = true;
};

static const SceneImportSettings DEFAULT_SCENE_IMPORT_SETTINGS{
    false, false, true, false, false, true, true, false, true};

} // namespace kirana::scene

#endif