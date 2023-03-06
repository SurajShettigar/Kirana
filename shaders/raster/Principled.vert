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
    int baseMap;
    int emissiveMap;
    int normalMap;
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
    gl_Position = getClipPosition();

    VSOut.worldPosition = getWorldPosition().xyz;
    VSOut.worldNormal = getWorldNormal();
    VSOut.viewDirection = camBuffer.c.direction;
    VSOut.matData = MaterialData(pushConstants.p.materialDataBufferAddress).p[pushConstants.p.materialDataIndex];
}