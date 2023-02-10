#version 460
#extension GL_GOOGLE_include_directive: enable

#include "base_vert.glsl"

struct BasicShadedData {
    vec4 color;
};

layout (buffer_reference) readonly buffer MaterialData {
    BasicShadedData b[];
};

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec3 outWorldNormal;
layout (location = 2) out vec3 outCamPos;

void main() {
    MaterialData mat = MaterialData(pushConstants.p.materialDataBufferAddress);
    BasicShadedData basicShaded = mat.b[pushConstants.p.materialDataIndex];

    gl_Position = getWorldPosition();

    outColor = basicShaded.color;
    outWorldNormal = getWorldNormal();
    outCamPos = camBuffer.c.position;
}