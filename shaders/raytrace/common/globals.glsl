#ifndef RAYTRACE_COMMON_GLOBALS_GLSL
#define RAYTRACE_COMMON_GLOBALS_GLSL 1

#extension GL_EXT_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int32 : require
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_buffer_reference2: require

// Shading Types
#define SHADING_TYPE_BASIC 0
#define SHADING_TYPE_PRINCIPLED 1
#define SHADING_TYPE_AO 2

precision highp float;

// Constants
const float PI = 3.14159265358979323;
const float TWO_PI = 6.28318530717958648; // 2PI
const float PI_BY_TWO = 1.57079632679489661923;   // PI/2
const float PI_BY_FOUR = 0.785398163397448309616;  // PI/4
const float ONE_BY_PI = 0.318309886183790671538;  // 1/PI
const float ONE_BY_TWO_PI = 0.15915494309189534;  // 1/2PI
const float ONE_BY_FOUR_PI = 0.07957747154594767;  // 1/4PI
const float TWO_BY_PI = 0.636619772367581343076;  // 2/PI
const float EPSILON = 0.0001;
const float INFINITY = 1e32;

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

struct PushConstantData {
    uint32_t frameIndex;
    uint32_t maxDepth;
    uint32_t maxSamples;
};

struct PathtraceParameters {
    uint32_t maxDepth;
};

struct Ray {
    vec3 origin;
    vec3 direction;
};

struct PathtracePayload {
    uint seed;
    float hitDistance;
    int primitiveID;
    int instanceID;
    int geometryIndex;
    int instanceCustomIndex;
    vec2 barycentricCoords;
    mat4x3 objectToWorldMat;
    mat4x3 worldToObjectMat;
};

struct IntersectionData {
    vec3 position;

    vec3 tangent;
    vec3 bitangent;
    vec3 normal;

    uint64_t materialBufferAddress;
    int materialIndex;
};

#endif


