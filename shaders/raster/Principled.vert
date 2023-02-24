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
    vec4 emissiveColor;
    float emissiveIntensity;
};

layout (buffer_reference) readonly buffer MaterialData {
    PrincipledData p[];
};

layout (location = 0) out _VSOut {
    vec3 worldPosition;
    vec3 worldNormal;
    vec3 viewDirection;
    PrincipledData matData;
} VSOut;

void main() {
    vec4 worldPos = getWorldPosition();
    gl_Position = worldPos;

    VSOut.worldPosition = worldPos.xyz;
    VSOut.worldNormal = getWorldNormal();
    VSOut.viewDirection = camBuffer.c.direction;
    VSOut.matData = MaterialData(pushConstants.p.materialDataBufferAddress).p[pushConstants.p.materialDataIndex];
}