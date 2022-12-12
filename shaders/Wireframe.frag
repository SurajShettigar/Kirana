#version 460

layout (location = 0) in vec3 inColor;
layout (location = 0) out vec4 outFragColor;

void main() {
    vec3 color = mix(vec3(0.75f, 0.75f, 0.75f), inColor, inColor);
    outFragColor = vec4(color, 1.0f);
}