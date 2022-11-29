layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vColor;

layout (set = 0, binding = 0) uniform CameraBuffer {
    mat4 view;
    mat4 proj;
    mat4 viewProj;
} camData;

layout (set = 0, binding = 1) uniform ModelBuffer {
    mat4 modelMatrix;
} modelData;

layout (push_constant) uniform constants
{
    mat4 modelMatrix;
} pushConstants;

vec4 getVertexPosition()
{
    return transpose(pushConstants.modelMatrix * camData.viewProj)
    * vec4(vPosition, 1.0f);
}

vec3 getWorldNormal()
{
    mat4 m = transpose(pushConstants.modelMatrix);
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

vec3 getWorldCameraPosition() {
    return vec3((camData.viewProj[0][3] / camData.viewProj[3][3],
    camData.viewProj[1][3] / camData.viewProj[3][3],
    camData.viewProj[2][3] / camData.viewProj[3][3]));
}