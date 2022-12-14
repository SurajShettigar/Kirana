#version 460
#extension GL_GOOGLE_include_directive: enable

#include "base_vert.glsl"

layout (location = 0) out vec3 outColor;

void main() {
    gl_Position = getVertexPosition();
    outColor = vColor;
}