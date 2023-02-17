#version 460
#extension GL_GOOGLE_include_directive: enable

#include "raytrace_types.glsl"

layout (set = 0, binding = 1) uniform _WorldData
{
    WorldData w;
} worldBuffer;

layout (location = 0) rayPayloadInEXT PathtracePayload payload;

void main()
{
    payload.color = worldBuffer.w.ambientColor.rgb * 8.0;
    payload.depth = 999;
}