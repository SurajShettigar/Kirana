#version 460
#extension GL_GOOGLE_include_directive: enable
#extension GL_EXT_buffer_reference2: enable

#include "raytrace_types.glsl"
#include "raytrace_utils.glsl"
#include "raytrace_hitdata.glsl"

struct BasicShadedData {
    vec4 color;
};

layout (set = 0, binding = 1) uniform _WorldData {
    WorldData w;
} worldBuffer;

layout (set = 1, binding = 0) readonly buffer _ObjectData {
    ObjectData o[];
} objBuffer;

layout (buffer_reference) readonly buffer MaterialData {
    BasicShadedData b[];
};

layout (location = 0) rayPayloadInEXT PathtracePayload payload;

hitAttributeEXT vec2 hitAttribs;

vec3 basicBRDF(in BasicShadedData matData, in HitPoint hitpoint)
{
    // Lambertian Diffuse BRDF
    return matData.color.rgb / PI;
}

void main()
{
    ObjectData obj = objBuffer.o[gl_InstanceCustomIndexEXT];
    HitPoint hitpoint = getHitPoint(obj, hitAttribs);

    MaterialData mBuffer = MaterialData(obj.materialDataBufferAddress);
    BasicShadedData matData = mBuffer.b[obj.materialDataIndex];

    vec3 rayOrigin = hitpoint.position + hitpoint.normal * EPSILON;
    vec3 reflectedDirection = normalize(randomHemispherical_Rejection(payload.seed,hitpoint.normal));

    payload.ray = Ray(rayOrigin, reflectedDirection);
    payload.color = vec3(0.0); // Light emmitted
    payload.weight = basicBRDF(matData, hitpoint) * abs(dot(hitpoint.normal, reflectedDirection)) * PI;
}