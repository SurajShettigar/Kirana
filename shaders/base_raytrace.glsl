#extension GL_EXT_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int32 : require
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require

struct HitInfo {
    vec3 color;
};

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

struct GlobalData {
    uint64_t vertexBufferAddress;
    uint64_t indexBufferAddress;
    uint32_t frameIndex;
};

struct Vertex {
    vec3 position;
    vec3 normal;
    vec4 color;
};

struct Object {
    uint32_t firstIndex;
    uint32_t vertexOffset;
};


