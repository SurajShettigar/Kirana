#extension GL_EXT_buffer_reference2: enable
#include "base_raster.glsl"

layout (set = 0, binding = 0) uniform _CameraData {
    CameraData c;
} camBuffer;

layout (buffer_reference) readonly buffer VertexData {
    Vertex v[];
};

layout (buffer_reference) readonly buffer IndexData {
    uint32_t i[];
};

layout (push_constant) uniform _PushConstantData {
    PushConstantData p;
} pushConstants;

Vertex getCurrentVertex()
{
    VertexData vBuffer = VertexData(pushConstants.p.vertexBufferAddress);
    IndexData iBuffer = IndexData(pushConstants.p.indexBufferAddress);
    uint32_t index = (pushConstants.p.firstIndex + gl_VertexIndex) + pushConstants.p.vertexOffset;
    return vBuffer.v[index];
}

vec3 getLocalVertexPosition()
{
    return getCurrentVertex().position;
}

vec4 getVertexPosition()
{
    return vec4(getCurrentVertex().position, 1.0f) * pushConstants.p.modelMatrix * camBuffer.c.viewProj;
}

vec3 getLocalNormal()
{
    return getCurrentVertex().normal;
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
    m[0][0] * m[1][1] - m[0][1] * m[1][0]) * getCurrentVertex().normal;
}

vec4 getVertexColor()
{
    return getCurrentVertex().color;
}

