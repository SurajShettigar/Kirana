#version 450
#extension GL_GOOGLE_include_directive: enable

#include "base_vert.glsl"

vec3 vertices[4] = vec3[4](
    vec3(-1.0, -1.0, 0.0f),
    vec3(-1.0, 1.0, 0.0f),
    vec3(1.0, 1.0, 0.0f),
    vec3(1.0, -1.0, 0.0f)
);

vec3 clipToWorldSpace(vec3 clipPos)
{
    vec4 pos = inverse(transpose(camData.viewProj)) * vec4(clipPos, 1.0f);
    return (pos.xyz / pos.w);
}

layout (location = 0) out vec3 nearPos;
layout (location = 1) out vec3 farPos;

void main() {
    vec3 pos = vertices[gl_VertexIndex];
    nearPos = clipToWorldSpace(vec3(pos.x, pos.y, 0.0));
    farPos = clipToWorldSpace(vec3(pos.x, pos.y, 1.0));
    gl_Position = vec4(pos, 1.0f);
}