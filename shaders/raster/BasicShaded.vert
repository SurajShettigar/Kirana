#version 460
#extension GL_GOOGLE_include_directive: enable

#include "../base/base_vert.glsl"

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec3 outWorldNormal;
layout (location = 2) out vec3 outCamPos;

void main() {
    gl_Position = getVertexPosition();
    outColor = vColor;
    outWorldNormal = getWorldNormal();
    outCamPos = camData.position;
}