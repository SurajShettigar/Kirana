#version 450
#extension GL_GOOGLE_include_directive: enable

#include "base_vert.glsl"

void main() {
    gl_Position = getVertexPosition();
}