#version 460
#extension GL_GOOGLE_include_directive: enable
#extension GL_EXT_ray_tracing : require

#include "base_raytrace.glsl"

layout (set = 0, binding = 1) uniform WorldData
{
    vec4 ambientColor;
    vec3 sunDirection;
    float sunIntensity;
    vec4 sunColor;
} worldData;
layout(location = 0) rayPayloadInEXT HitInfo payload;

void main()
{
    payload.color = worldData.ambientColor.xyz;
}