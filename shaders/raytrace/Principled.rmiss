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
    float yVal = clamp((1.0 - float(gl_LaunchIDEXT.y) / float(gl_LaunchSizeEXT.y)) * 1.5, 0.0, 1.0);
    payload.color = mix(vec3(0.2, 0.1, 0.0), vec3(0.85, 0.9, 0.95), yVal);
//    payload.color = worldBuffer.w.ambientColor.rgb * 3.0;
    payload.depth = 999;
}