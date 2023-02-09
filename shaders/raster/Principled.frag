#version 460

layout (location = 0) out vec4 outFragColor;


layout (location = 0) in vec4 inColor;

void main() {
    outFragColor = inColor;
}