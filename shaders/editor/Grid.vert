#version 460
#extension GL_GOOGLE_include_directive: enable

#include "base_vert.glsl"

vec3 vertices[4] = vec3[4](
    vec3(-1.0, -1.0, 0.0),
    vec3(-1.0, 1.0, 0.0),
    vec3(1.0, 1.0, 0.0),
    vec3(1.0, -1.0, 0.0)
);

vec3 clipToWorldSpace(vec3 clipPos)
{
    vec4 pos = vec4(clipPos, 1.0f) * camBuffer.c.invViewProj;
    return (pos.xyz / pos.w);
}

layout(location=0) out VSOut {
    mat4 viewProj;
    vec3 nearPos;
    vec3 farPos;
    float camNear;
    float camFar;
    vec3 camPos;
}vsOut;

void main() {
    vec3 pos = vertices[gl_VertexIndex];
    vsOut.viewProj = camBuffer.c.viewProj;
    vsOut.nearPos = clipToWorldSpace(vec3(pos.x, pos.y, 0.0));
    vsOut.farPos = clipToWorldSpace(vec3(pos.x, pos.y, 1.0));
    vsOut.camNear = camBuffer.c.nearPlane;
    vsOut.camFar = camBuffer.c.farPlane;
    vsOut.camPos = camBuffer.c.position;
    gl_Position = vec4(pos, 1.0f);
}