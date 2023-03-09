#version 460

#extension GL_GOOGLE_include_directive: enable

#include "base_raster.glsl"

layout (location = 0) in vec4 inColor;
layout (location = 1) in vec3 inWorldNormal;
layout (location = 2) in vec3 inCamPos;
layout (location = 0) out vec4 outFragColor;

layout (set = 0, binding = 1) uniform _WorldData {
    WorldData w;
} worldBuffer;

void main() {
    vec3 color = inColor.rgb;
    color += worldBuffer.w.ambientColor.rgb;
    color *= max(dot(inWorldNormal, normalize(- worldBuffer.w.sunDirection)), 0.075f)
    * worldBuffer.w.sunColor.rgb;
    outFragColor = vec4(color, 1.0);
}