#version 460
#extension GL_GOOGLE_include_directive: enable
#extension GL_EXT_buffer_reference2: enable

#include "base_vert.glsl"

struct OutlineData {
    vec4 color;
    float thickness;
};

layout (buffer_reference) readonly buffer MaterialData {
    OutlineData o[];
};

layout (location = 0) out vec4 outColor;

void main() {
    MaterialData mat = MaterialData(pushConstants.p.materialDataBufferAddress);
    OutlineData outline = mat.o[pushConstants.p.materialDataIndex];

    float thickness = outline.thickness;
    vec3 pos = vPosition + vNormal * thickness;
    gl_Position = vec4(pos, 1.0f) * pushConstants.p.modelMatrix * camBuffer.c.viewProj;

    outColor = outline.color;
}