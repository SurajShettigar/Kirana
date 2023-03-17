#ifndef KIRANA_SCENE_SCENE_TYPES_HPP
#define KIRANA_SCENE_SCENE_TYPES_HPP

#include <transform.hpp>
#include <vector2.hpp>
#include <array>

namespace kirana::scene
{
typedef uint32_t INDEX_TYPE;

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

enum class ObjectType
{
    EMPTY = 0,
    MESH = 1, LIGHT = 2,
    CAMERA = 3,
};

struct Node
{
    int parent = -1;
    int child = -1;
    int sibling = -1;
    uint32_t level = 0;
    ObjectType objectType = ObjectType::EMPTY;
    int objectIndex = -1;
    int transformIndex = -1;
};

} // namespace kirana::scene

#endif