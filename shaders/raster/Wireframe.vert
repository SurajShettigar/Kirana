#version 460
#extension GL_GOOGLE_include_directive: enable

#include "base_vert.glsl"

struct WireframeData {
    vec4 color;
};

layout (buffer_reference) readonly buffer MaterialData {
    WireframeData w[];
};

layout (location = 0) out vec4 outColor;

void main() {
    MaterialData mat = MaterialData(pushConstants.p.materialDataBufferAddress);
    WireframeData wireframe = mat.w[pushConstants.p.materialDataIndex];

    gl_Position = getClipPosition();
    outColor = wireframe.color;
}