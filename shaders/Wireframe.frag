#version 460

layout (location = 0) in vec4 inColor;
layout (location = 0) out vec4 outFragColor;

void main() {
    outFragColor = mix(vec4(0.75, 0.75, 0.75, 1.0), inColor, inColor);
}