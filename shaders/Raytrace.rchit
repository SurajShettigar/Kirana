#version 460
#extension GL_GOOGLE_include_directive: enable
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_shader_explicit_arithmetic_types_int32 : require
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
#extension GL_EXT_buffer_reference2 : require
#extension GL_EXT_scalar_block_layout : enable

#include "base_raytrace.glsl"

struct ObjectData {
    mat4 modelMatrix; // Row-major
    uint64_t vertexBufferAddress;
    uint64_t indexBufferAddress;
    vec3 color;
};

struct Vertex {
    vec3 position;
    vec3 normal;
    vec3 color;
};

layout (buffer_reference) buffer VertexBuffer {
    Vertex vertices[];
};
layout (buffer_reference) buffer IndexBuffer {
    uint32_t indices[];
};

layout (set = 0, binding = 1) uniform WorldData
{
    vec4 ambientColor;
    vec3 sunDirection;
    float sunIntensity;
    vec4 sunColor;
} worldData;

layout (set = 1, binding = 0) readonly buffer ObjectBuffer {
    ObjectData objects[];
} objectBuffer;


layout (location = 0) rayPayloadInEXT HitInfo payload;
hitAttributeEXT vec3 attribs;

void main()
{
    ObjectData o = objectBuffer.objects[gl_InstanceCustomIndexEXT];
    VertexBuffer vb = VertexBuffer(o.vertexBufferAddress);
    IndexBuffer ib = IndexBuffer(o.indexBufferAddress);

    uint32_t i01 = ib.indices[gl_PrimitiveID * 3];
    uint32_t i02 = ib.indices[gl_PrimitiveID * 3 + 1];
    uint32_t i03 = ib.indices[gl_PrimitiveID * 3 + 2];

    const Vertex v0 = vb.vertices[i01];
    const Vertex v1 = vb.vertices[i02];
    const Vertex v2 = vb.vertices[i03];

    const vec3 barycentrics = vec3(1.0 - attribs.x - attribs.y, attribs.x, attribs.y);

    const vec3 pos = v0.position.xyz * barycentrics.x + v1.position.xyz * barycentrics.y + v2.position.xyz * barycentrics.z;
    const vec3 worldPos = vec3(gl_ObjectToWorldEXT * vec4(pos, 1.0));

    const vec3 normal = v0.normal.xyz * barycentrics.x + v1.normal.xyz * barycentrics.y + v2.normal.xyz * barycentrics.z;
    const vec3 worldNormal = vec3(normalize(normal * gl_WorldToObjectEXT));

    vec3 color = vec3(0.65, 0.65, 0.65);
//    color *= worldData.ambientColor.rbg;
//    color *= dot(worldNormal, normalize(- worldData.sunDirection)) * worldData.sunColor.rgb * worldData.sunIntensity;

    payload.color = color;
}
