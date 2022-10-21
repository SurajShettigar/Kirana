#version 450

layout (location = 0) in vec3 inColor;
layout (location = 1) in vec3 inWorldNormal;
layout (location = 2) in vec3 inCamPos;
layout (location = 0) out vec4 outFragColor;

layout (set = 0, binding = 1) uniform WorldData
{
    vec4 ambientColor;
    vec3 sunDirection;
    float sunIntensity;
    vec4 sunColor;
} worldData;

void main() {
    //    vec3 color = mix(vec3(0.5f, 0.5f, 0.5f), inColor, inColor);
    vec3 color = vec3(0.5f, 0.5f, 0.5f);
    color = pow(abs((1.0f - dot(inWorldNormal, inCamPos))), 0.75f) * color;
    color *= worldData.ambientColor.rgb;
    color *= max(dot(inWorldNormal, normalize(-worldData.sunDirection)), 0.005f) * worldData.sunColor.rgb * worldData.sunIntensity;
    outFragColor = vec4(clamp(color, 0.0f, 1.0f), 1.0f);
}