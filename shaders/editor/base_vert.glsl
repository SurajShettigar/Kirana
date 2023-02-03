#extension GL_EXT_buffer_reference2: enable

#include "base_raster.glsl"

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec4 vColor;

layout (set = 0, binding = 0) uniform _CameraData {
    CameraData c;
} camBuffer;

layout (push_constant) uniform _PushConstantData {
    PushConstantData p;
} pushConstants;

vec4 getWorldPosition()
{
    return vec4(vPosition, 1.0f) * pushConstants.p.modelMatrix * camBuffer.c.viewProj;
}

vec3 getWorldNormal()
{
    mat4 m = transpose(pushConstants.p.modelMatrix);
    return mat3(m[1][1] * m[2][2] - m[1][2] * m[2][1],
    m[1][2] * m[2][0] - m[1][0] * m[2][2],
    m[1][0] * m[2][1] - m[1][1] * m[2][0],
    m[0][2] * m[2][1] - m[0][1] * m[2][2],
    m[0][0] * m[2][2] - m[0][2] * m[2][0],
    m[0][1] * m[2][0] - m[0][0] * m[2][1],
    m[0][1] * m[1][2] - m[0][2] * m[1][1],
    m[0][2] * m[1][0] - m[0][0] * m[1][2],
    m[0][0] * m[1][1] - m[0][1] * m[1][0]) * vNormal;
}

vec4 getVertexColor()
{
    return vColor;
}

