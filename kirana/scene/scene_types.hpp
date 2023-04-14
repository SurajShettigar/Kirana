#ifndef KIRANA_SCENE_SCENE_TYPES_HPP
#define KIRANA_SCENE_SCENE_TYPES_HPP

#include <transform.hpp>
#include <vector2.hpp>
#include <id_manager.hpp>
#include <event.hpp>

#include <array>
#include <string>

namespace kirana::scene
{
typedef uint32_t INDEX_TYPE;
typedef utils::ID ObjectID;

enum class VertexAttributeFormat
{
    INT = 0,
    FLOAT = 1,
};

struct VertexAttributeInfo
{
    VertexAttributeFormat format;
    uint8_t componentCount;
    size_t structOffset;
};

struct Vertex
{
    math::Vector3 position;
    alignas(16) math::Vector3 normal;
    alignas(16) math::Vector4 color;
    math::Vector2 texCoords;

    /// Vertex attribute information to create vertex bindings.
    static std::vector<VertexAttributeInfo> getVertexInfo()
    {
        return {
            {VertexAttributeFormat::FLOAT, 3, offsetof(Vertex, position)},
            {VertexAttributeFormat::FLOAT, 3, offsetof(Vertex, normal)},
            {VertexAttributeFormat::FLOAT, 4, offsetof(Vertex, color)},
            {VertexAttributeFormat::FLOAT, 2, offsetof(Vertex, texCoords)},
        };
    }
};

enum class NodeObjectType
{
    EMPTY = 0,
    MESH = 1,
    LIGHT = 2,
    CAMERA = 3,
};

struct NodeObjectData
{
    NodeObjectType type = NodeObjectType::EMPTY;
    int objectIndex = -1;
    int transformIndex = -1;
};

struct NodeRenderData
{
    bool selectable = true;
    bool viewportVisible = true;
    bool renderVisible = true;
    bool selected = false;
};

struct Node
{
    // Graph data
    int index = -1;
    int parent = -1;
    int child = -1;
    int sibling = -1;
    uint32_t level = 0;
    NodeObjectData objectData;
    NodeRenderData renderData;
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
    bool generateUVs = true;
    bool transformUVs = false;
    bool flipUVs = false;
};

struct SceneStatistics
{
    uint32_t vertexSize = 0;
    uint32_t numVertices = 0;
    uint32_t indexSize = 0;
    uint32_t numIndices = 0;
    uint32_t numMeshes = 0;
    uint32_t numMaterials = 0;
    uint32_t numLights = 0;
    uint32_t numCameras = 0;

    [[nodiscard]] inline size_t getTotalVertexSize() const
    {
        return static_cast<size_t>(vertexSize) * numVertices;
    }

    [[nodiscard]] inline size_t getTotalIndexSize() const
    {
        return static_cast<size_t>(indexSize) * numIndices;
    }

    explicit operator std::string() const
    {
        return "{Vertex Size: " + std::to_string(vertexSize) + ", " +
               "Num. Vertices: " + std::to_string(numVertices) + ", " +
               "Total Vertex Size: " + std::to_string(getTotalVertexSize()) +
               ", " + "Index Size: " + std::to_string(indexSize) + ", " +
               "Num. Indices: " + std::to_string(numIndices) + ", " +
               "Total Index Size: " + std::to_string(getTotalIndexSize()) +
               ", " + "Num. Meshes: " + std::to_string(numMeshes) + ", " +
               "Num. Materials: " + std::to_string(numMaterials) + ", " +
               "Num. Lights: " + std::to_string(numLights) + ", " +
               "Num. Cameras: " + std::to_string(numCameras) + "}";
    }
};

} // namespace kirana::scene

#endif