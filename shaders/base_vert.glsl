// Vertex Buffer input
layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec3 vColor;

// Descriptor Set Buffer input
layout (set = 0, binding = 0) uniform CameraBuffer {
    mat4 view; // Row-major storage
    mat4 proj; // Row-major
    mat4 viewProj; // Row-major
    vec3 position;
    vec3 direction;
    float nearPlane;
    float farPlane;
} camData;

struct ObjectData {
    mat4 modelMatrix; // Row-major
};

layout(std140,set = 1, binding = 0) readonly buffer ObjectBuffer{
    ObjectData objects[];
} objectBuffer;


vec4 getVertexPosition()
{
    return transpose(objectBuffer.objects[gl_BaseInstance].modelMatrix * camData.viewProj)
    * vec4(vPosition, 1.0f);
}

vec3 getWorldNormal()
{
    mat4 m = transpose(objectBuffer.objects[gl_BaseInstance].modelMatrix);
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