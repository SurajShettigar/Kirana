#version 460
#extension GL_GOOGLE_include_directive : enable
#extension GL_EXT_buffer_reference2 : require

#include "base_raytrace.glsl"

layout (set = 0, binding = 1) uniform _WorldData {
    WorldData w;
} worldBuffer;

layout (set = 1, binding = 0) uniform accelerationStructureEXT topLevelAS;
layout (set = 1, binding = 2) readonly buffer ObjectData {
    Object o[];
} objBuffer;


layout (push_constant) uniform _GlobalData {
    GlobalData g;
} globalBuffer;

layout (buffer_reference) readonly buffer VertexData {
    Vertex v[];
};
layout (buffer_reference) readonly buffer IndexData {
    uint32_t i[];
};

layout (location = 0) rayPayloadInEXT HitInfo payload;
layout (location = 1) rayPayloadEXT bool isShadowed;

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
    return vec3(normalize(normal * gl_WorldToObjectEXT));
}


void main()
{
    VertexData vBuffer = VertexData(globalBuffer.g.vertexBufferAddress);
    IndexData iBuffer = IndexData(globalBuffer.g.indexBufferAddress);

    const u32vec3 indices = getTriangleIndices(iBuffer);
    Vertex v[3] = getTriangles(vBuffer, indices);

    const vec3 barycentrics = vec3(1.0 - attribs.x - attribs.y, attribs.x, attribs.y);
    const vec3 worldPos = getWorldPosition(vec3[3](v[0].position, v[1].position, v[2].position), barycentrics);
    const vec3 worldNormal = getWorldNormal(vec3[3](v[0].normal, v[1].normal, v[2].normal), barycentrics);

    float attenuation = 1.0;
    const float lightDistance = 100000.0;
    const vec3 lightDirection = normalize(- worldBuffer.w.sunDirection);
    const vec3 lightColor = worldBuffer.w.sunColor.rgb * worldBuffer.w.sunIntensity;
    const float dotL = dot(worldNormal, lightDirection);

    if (dotL > 0.0)
    {
        float tMin = 0.001;
        float tMax = lightDistance;
        vec3 origin = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
        vec3 direction = lightDirection;
        uint flags = gl_RayFlagsOpaqueEXT + gl_RayFlagsTerminateOnFirstHitEXT + gl_RayFlagsSkipClosestHitShaderEXT;
        isShadowed = true;
        traceRayEXT(topLevelAS, // Acceleration Structure
        flags, // Ray flags
        0xFF, // Culling Mask. ANDed with mask set in acceleration structure.
        0, // SBT Record offset
        0, // SBT Record stride
        1, // Miss Shader Index
        origin, // Ray origin
        tMin, // Min Ray hit distance
        direction, // Ray direction
        tMax, // Max Ray hit distance
        1 // Payload location
        );
    }

    if (isShadowed)
    attenuation = 0.1;

    vec3 color = vec3(0.65, 0.65, 0.65);
    color += worldBuffer.w.ambientColor.rbg;
    color *= max(dotL, 0.0) * lightColor * attenuation;

    payload.color = color.rgb;
}
