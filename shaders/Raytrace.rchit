#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable
#extension GL_GOOGLE_include_directive : enable

#extension GL_EXT_shader_explicit_arithmetic_types_int32 : require
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_buffer_reference2 : require

#include "base_raytrace.glsl"

struct Vertex {
    vec4 position;
    vec4 normal;
    vec4 color;
};

layout (set = 0, binding = 1) uniform WorldData
{
    vec4 ambientColor;
    vec3 sunDirection;
    float sunIntensity;
    vec4 sunColor;
} worldData;

struct ObjectData {
    mat4 localMat;
    uint firstIndex;
    uint vertexOffset;
    uint padding1;
    uint padding2;
};

layout (set = 1, binding = 3) readonly buffer ObjectBuffer {
    ObjectData objects[];
} objBuffer;

layout (set = 1, binding = 4) readonly buffer VertexBuffer {
    Vertex v[];
} vBuffer;
layout (set = 1, binding = 5) readonly buffer IndexBuffer {
    uint32_t i[];
} iBuffer;

layout (location = 0) rayPayloadInEXT HitInfo payload;
hitAttributeEXT vec2 attribs;

void main()
{
    ObjectData obj = objBuffer.objects[gl_InstanceCustomIndexEXT];
    uint indexOffset = obj.firstIndex + (3 * gl_PrimitiveID);
    uint vertexOffset = obj.vertexOffset;

    u32vec3 index = u32vec3(iBuffer.i[indexOffset], iBuffer.i[indexOffset + 1], iBuffer.i[indexOffset + 2]);
    index += u32vec3(vertexOffset);

    Vertex v0 = vBuffer.v[index.x];
    Vertex v1 = vBuffer.v[index.y];
    Vertex v2 = vBuffer.v[index.z];

    const vec3 barycentrics = vec3(1.0 - attribs.x - attribs.y, attribs.x, attribs.y);

    const vec4 pos = v0.position * barycentrics.x + v1.position * barycentrics.y + v2.position * barycentrics.z;
    const vec3 worldPos = vec3(gl_ObjectToWorldEXT * vec4(pos.xyz, 1.0));

    vec3 normal = v0.normal.xyz * barycentrics.x + v1.normal.xyz * barycentrics.y + v2.normal.xyz * barycentrics.z;
    normal = normalize(normal);
    vec3 worldNormal = vec3(normalize(normal * gl_WorldToObjectEXT));

    vec3 color = vec3(0.65, 0.65, 0.65);
    color += worldData.ambientColor.rbg;
    color *= worldData.sunColor.rgb * max(dot(worldNormal, normalize(-worldData.sunDirection)), 0.0);

    payload.color = worldPos.rgb;
}
