#version 460
#extension GL_GOOGLE_include_directive: enable

#include "base_vert.glsl"

void main() {
    // TODO: Take outline thickness from input buffer
    float thickness = 0.01;
    vec3 pos = getLocalVertexPosition() + getLocalNormal() * thickness;
    gl_Position = vec4(pos, 1.0f) * pushConstants.p.modelMatrix * camBuffer.c.viewProj;
}