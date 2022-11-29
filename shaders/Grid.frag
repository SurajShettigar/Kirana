#version 450
layout(location=0) in vec3 nearPos;
layout(location=1) in vec3 farPos;
layout (location = 0) out vec4 outFragColor;

vec4 grid(vec3 pos, float scale) {
    vec2 coord = pos.xz * scale; // use the scale variable to set the distance between the lines
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);
    vec4 color = vec4(0.2, 0.2, 0.2, 1.0 - min(line, 1.0));
    // z axis
    if(pos.x > -0.1 * minimumx && pos.x < 0.1 * minimumx)
    color.z = 1.0;
    // x axis
    if(pos.z > -0.1 * minimumz && pos.z < 0.1 * minimumz)
    color.x = 1.0;
    return color;
}

void main() {
    float t = -nearPos.y / (farPos.y - nearPos.y);
    vec3 pos = nearPos + t * (farPos - nearPos);
    outFragColor = grid(pos, 1) * (t > 0.0f ? 1.0f : 0.0f);
}