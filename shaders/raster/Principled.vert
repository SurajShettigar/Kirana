#version 460
#extension GL_GOOGLE_include_directive: enable

#include "base_vert.glsl"

struct PrincipledData {
    vec4 color;
    float subSurface;
    float metallic;
    float specular;
    float specularTint;
    float roughness;
    float anisotropic;
    float sheen;
    float sheenTint;
    float clearCoat;
    float clearCoatGloss;
    float transmission;
    float ior;
};

layout (buffer_reference) readonly buffer MaterialData {
    PrincipledData p[];
};

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec3 outWorldNormal;
layout (location = 2) out vec3 outCamPos;

void main() {
    MaterialData mat = MaterialData(pushConstants.p.materialDataBufferAddress);
    PrincipledData principled = mat.p[pushConstants.p.materialDataIndex];

    gl_Position = getWorldPosition();

    outColor = principled.color;
    outWorldNormal = getWorldNormal();
    outCamPos = camBuffer.c.position;
}