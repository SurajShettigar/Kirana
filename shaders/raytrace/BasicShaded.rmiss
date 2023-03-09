#version 460
#extension GL_GOOGLE_include_directive: enable

#include "common/globals.glsl"

layout (location = 0) rayPayloadInEXT PathtracePayload payload;

void main()
{
    payload.hitDistance = INFINITY;
}