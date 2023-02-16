#extension GL_EXT_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int32 : require
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require

struct Vertex {
    vec3 position;
    vec3 normal;
    vec4 color;
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

struct ObjectData {
    uint64_t vertexBufferAddress;
    uint64_t indexBufferAddress;
    uint64_t materialDataBufferAddress;
    int materialDataIndex;
    uint32_t firstIndex;
    uint32_t vertexOffset;
};

struct GlobalData {
    uint32_t frameIndex;
    uint32_t maxBounces;
    uint32_t antiAliasSamples;
};

struct Ray {
    vec3 origin;
    vec3 direction;
};

struct PathtracePayload {
    Ray ray;
    vec3 color;
    vec3 weight;
    uint seed;
    uint depth;
};

struct HitPoint {
    vec3 position;
    vec3 tangent;
    vec3 bitangent;
    vec3 normal;
};

const float PI = 3.141592;
const float EPSILON = 0.00001;


