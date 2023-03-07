#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int32 : require
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require

struct CameraData {
    mat4 view; // Row-major storage
    mat4 proj; // Row-major
    mat4 viewProj; // Row-major
    mat4 invViewProj; // Row-major
    vec3 position;
    vec3 direction;
    float nearPlane;
    float farPlane;
};

struct WorldData
{
    vec4 ambientColor;
    vec3 sunDirection;
    float sunIntensity;
    vec4 sunColor;
};

struct Vertex {
    vec3 position;
    vec3 normal;
    vec4 color;
    vec2 texCoords;
};

struct PushConstantData {
    mat4x4 modelMatrix;
    uint64_t vertexBufferAddress;
    uint64_t indexBufferAddress;
    uint64_t materialDataBufferAddress;
    int materialDataIndex;
    uint32_t objectIndex;
    uint32_t firstIndex;
    uint32_t vertexOffset;
};