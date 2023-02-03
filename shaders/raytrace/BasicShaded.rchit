#version 460
#extension GL_GOOGLE_include_directive: enable
#extension GL_EXT_buffer_reference2: enable

#include "base_raytrace.glsl"
#include "raytrace_utils.glsl"

layout (set = 0, binding = 1) uniform _WorldData {
    WorldData w;
} worldBuffer;

layout (set = 1, binding = 0) readonly buffer ObjectData {
    Object o[];
} objBuffer;

layout (buffer_reference) readonly buffer VertexData {
    Vertex v[];
};
layout (buffer_reference) readonly buffer IndexData {
    uint32_t i[];
};

layout (push_constant) uniform _GlobalData {
    GlobalData g;
} globalConstants;

layout (location = 0) rayPayloadInEXT HitInfo payload;

hitAttributeEXT vec2 attribs;

u32vec3 getTriangleIndices(IndexData iBuffer)
{
    const Object obj = objBuffer.o[gl_InstanceCustomIndexEXT];
    uint32_t indexOffset = obj.firstIndex + (3 * gl_PrimitiveID);
    uint32_t vertexOffset = obj.vertexOffset;

    u32vec3 index = u32vec3(iBuffer.i[indexOffset], iBuffer.i[indexOffset + 1], iBuffer.i[indexOffset + 2]);
    index += u32vec3(vertexOffset);
    return index;
}

Vertex[3] getTriangles(VertexData vBuffer, const u32vec3 indices)
{
    return Vertex[3](vBuffer.v[indices.x], vBuffer.v[indices.y], vBuffer.v[indices.z]);
}

vec3 getWorldPosition(const vec3[3] vPositions, const vec3 barycentrics)
{
    const vec3 pos = vPositions[0] * barycentrics.x + vPositions[1] * barycentrics.y + vPositions[2] * barycentrics.z;
    return vec3(gl_ObjectToWorldEXT * vec4(pos, 1.0));
}

vec3 getWorldNormal(const vec3[3] vNormals, vec3 barycentrics)
{
    const vec3 normal = vNormals[0] * barycentrics.x + vNormals[1] * barycentrics.y + vNormals[2] * barycentrics.z;
    return normalize(vec3(normal * gl_WorldToObjectEXT));
}

void main()
{
    const float PI = 3.141592;
    VertexData vBuffer = VertexData(objBuffer.o[gl_InstanceCustomIndexEXT].vertexBufferAddress);
    IndexData iBuffer = IndexData(objBuffer.o[gl_InstanceCustomIndexEXT].indexBufferAddress);

    const u32vec3 indices = getTriangleIndices(iBuffer);
    Vertex v[3] = getTriangles(vBuffer, indices);

    const vec3 barycentrics = vec3(1.0 - attribs.x - attribs.y, attribs.x, attribs.y);
    const vec3 worldPos = getWorldPosition(vec3[3](v[0].position, v[1].position, v[2].position), barycentrics);
    const vec3 worldNormal = getWorldNormal(vec3[3](v[0].normal, v[1].normal, v[2].normal), barycentrics);

    vec3 tangent;
    vec3 binormal;
    getCoordinateFrame(worldNormal, tangent, binormal);

    const vec3 reflectedDirection = normalize(randomHemispherical(payload.seed, vec3[3](tangent, binormal, worldNormal)));
    const vec3 matColor = vec3(1.0);
    vec3 brdf = matColor / PI;

    payload.rayOrigin = worldPos;
    payload.rayDirection = reflectedDirection;
    payload.color = vec3(0.0); // Light emmitted
    payload.weight = brdf * dot(worldNormal, reflectedDirection) * PI;
}