#version 460
#extension GL_GOOGLE_include_directive: enable

#include "base_raytrace.glsl"

layout (set = 0, binding = 1) uniform _WorldData
{
    WorldData w;
} worldBuffer;

layout (location = 0) rayPayloadInEXT HitInfo payload;

void main()
{
    payload.color = vec3(4.0, 4.0, 4.0);
    payload.depth = 999;
}