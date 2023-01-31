#ifndef SCENE_UTILS_HPP
#define SCENE_UTILS_HPP

#include <transform.hpp>

namespace kirana::scene
{
class Object;
class Material;

enum class VertexDataFormat
{
    INT = 0,
    FLOAT = 1,
};

struct VertexInfo
{
    VertexDataFormat format;
    uint8_t componentCount;
    size_t structOffset;
};

struct Vertex
{
    math::Vector3 position;
    alignas(16) math::Vector3 normal;
    alignas(16) math::Vector4 color;

    /// Vertex attribute information to create vertex bindings.
    static std::vector<VertexInfo> getVertexInfo()
    {
        return {
            {VertexDataFormat::FLOAT, 3, offsetof(Vertex, position)},
            {VertexDataFormat::FLOAT, 3, offsetof(Vertex, normal)},
            {VertexDataFormat::FLOAT, 4, offsetof(Vertex, color)},
        };
    }

    /// Vertex information of the biggest attribute
    static VertexInfo getLargestVertexInfo()
    {
        return {VertexDataFormat::FLOAT, 4, offsetof(Vertex, color)};
    }
};

typedef uint32_t INDEX_TYPE;

struct WorldData
{
    math::Vector4 ambientColor{0.1f, 0.1f, 0.1f, 1.0f};
    math::Vector3 sunDirection{0.25f, -0.75f, -0.25f};
    alignas(4) float sunIntensity{1.0f};
    alignas(16) math::Vector4 sunColor{1.0f, 1.0f, 1.0f, 1.0f};
};

struct CameraData
{
    math::Matrix4x4 viewMatrix;
    math::Matrix4x4 projectionMatrix;
    math::Matrix4x4 viewProjectionMatrix;
    math::Matrix4x4 invViewProjMatrix;
    math::Vector3 position;
    alignas(16) math::Vector3 direction;
    alignas(4) float nearPlane;
    alignas(4) float farPlane;
};

struct SceneInfo
{
    uint32_t vertexSize = 0;
    uint32_t numVertices = 0;
    size_t totalVertexSize = 0;
    uint32_t indexSize = 0;
    uint32_t numIndices = 0;
    size_t totalIndexSize = 0;
};

struct Renderable
{
    const Object *object = nullptr;
    bool selectable = true;
    bool viewportVisible = true;
    bool renderVisible = true;
    bool selected = false;
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

static const SceneImportSettings DEFAULT_SCENE_IMPORT_SETTINGS{
    false, false, true, false, false, true, true, false, true};

} // namespace kirana::scene

#endif