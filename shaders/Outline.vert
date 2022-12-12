#version 460
#extension GL_GOOGLE_include_directive: enable

#include "base_vert.glsl"

void main() {
    // TODO: Take outline thickness from input buffer
    float thickness = 0.01;
    vec3 pos = vPosition + vNormal * thickness;
    gl_Position = transpose(objectBuffer.objects[gl_BaseInstance].modelMatrix * camData.viewProj) * vec4(pos, 1.0f);
}