#version 460
#extension GL_GOOGLE_include_directive: enable
#extension GL_EXT_buffer_reference2: enable

#include "raytrace_types.glsl"
#include "raytrace_utils.glsl"
#include "raytrace_hitdata.glsl"

struct PrincipledData {
    vec4 color;
    float subSurface;
    float metallic;
    float specular;
    float specularTint;
    float roughness;
    float anisotropic;
    float sheen;
    float sheenTint;
    float clearCoat;
    float clearCoatGloss;
    float transmission;
    float ior;
};

layout (set = 0, binding = 1) uniform _WorldData {
    WorldData w;
} worldBuffer;

layout (set = 1, binding = 0) readonly buffer _ObjectData {
    ObjectData o[];
} objBuffer;

layout (buffer_reference) readonly buffer MaterialData {
    PrincipledData p[];
};

layout (location = 0) rayPayloadInEXT PathtracePayload payload;

hitAttributeEXT vec2 hitAttribs;

vec3 principledBRDF(in PrincipledData matData, in HitPoint hitpoint)
{
    // Lambertian Diffuse BRDF
    return matData.color.rgb / PI;
}

void main()
{
    ObjectData obj = objBuffer.o[gl_InstanceCustomIndexEXT];
    HitPoint hitpoint = getHitPoint(obj, hitAttribs);

    MaterialData mBuffer = MaterialData(obj.materialDataBufferAddress);
    PrincipledData matData = mBuffer.p[obj.materialDataIndex];

    vec3 rayOrigin = hitpoint.position + hitpoint.normal * EPSILON;
    const vec3 reflectedDirection = normalize(randomHemispherical(payload.seed,
    vec3[3](hitpoint.tangent, hitpoint.bitangent, hitpoint.normal)));

    payload.ray = Ray(rayOrigin, reflectedDirection);
    payload.color = vec3(0.0); // Light emmitted
    payload.weight = principledBRDF(matData, hitpoint) * abs(dot(hitpoint.normal, reflectedDirection)) * PI;
}