#version 460
#extension GL_GOOGLE_include_directive: enable

#include "base_vert.glsl"

layout (location = 0) out _VSOut {
    vec3 worldPosition;
    vec3 worldNormal;
    vec3 worldTangent;
    vec3 worldBitangent;
    vec3 viewDirection;
    vec2 texCoords;
    flat uint64_t matBufferAdd;
    flat uint matDataIndex;
} VSOut;

void main() {
    gl_Position = getClipPosition();

    VSOut.worldPosition = getWorldPosition().xyz;
    VSOut.worldNormal = getWorldNormal();
    getCoordinateFrame(VSOut.worldNormal, VSOut.worldTangent, VSOut.worldBitangent);
    VSOut.viewDirection = camBuffer.c.direction;
    VSOut.texCoords = vTexCoords;
    VSOut.matBufferAdd = pushConstants.p.materialDataBufferAddress;
    VSOut.matDataIndex = uint(pushConstants.p.materialDataIndex);
}