#version 450
#extension GL_GOOGLE_include_directive: enable

#include "base_vert.glsl"

void main() {
    // TODO: Take outline thickness from input buffer
    vec3 thickness = vec3(0.025);
    mat4 scaleMat = mat4(1.0 + thickness.x, 0.0, 0.0, 0.0,
    0.0, 1.0 + thickness.y, 0.0, 0.0,
    0.0, 0.0, 1.0 + thickness.z, 0.0,
    0.0, 0.0, 0.0, 1.0);
    gl_Position = transpose(scaleMat * pushConstants.modelMatrix * camData.viewProj) * vec4(vPosition, 1.0f);
}