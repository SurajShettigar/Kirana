#version 450

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vColor;
layout (location = 0) out vec3 outColor;
layout (location = 1) out vec3 outWorldNormal;
layout (location = 2) out vec3 outCamPos;

layout (set = 0, binding = 0) uniform CameraBuffer {
    mat4 view;
    mat4 proj;
} camData;

layout (set = 0, binding = 1) uniform ModelBuffer {
    mat4 modelMatrix;
} modelData;

layout (push_constant) uniform constants
{
    mat4 modelMatrix;
} pushConstants;

void main() {
    mat4 viewProj = camData.view * camData.proj;
    mat4 transformMatrix = transpose(pushConstants.modelMatrix * viewProj);
    gl_Position = transformMatrix * vec4(vPosition, 1.0f);
    outColor = vColor;
    outWorldNormal = vec3(transpose(pushConstants.modelMatrix) * vec4(vNormal, 0.0f));
    outCamPos = vec3(viewProj[0][3] / viewProj[3][3], viewProj[1][3] / viewProj[3][3], viewProj[2][3] / viewProj[3][3]);
}