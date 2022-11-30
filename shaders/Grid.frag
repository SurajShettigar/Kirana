#version 450

layout (location = 0) in VSIn {
    mat4 viewProj;
    vec3 nearPos;
    vec3 farPos;
    float camNear;
    float camFar;
    vec3 camPos;
} vsIn;

layout (location = 0) out vec4 outFragColor;

// Taken from: https://asliceofrendering.com/scene%20helper/2020/01/05/InfiniteGrid/

vec4 grid(vec3 pos, vec4 lineColor, float scale, bool drawAxis) {
    vec2 coord = pos.xz * scale;
    vec2 derivative = fwidth(coord) * 1.5; // Change in co-ordinate values. Used for AA
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1.0);
    float minimumx = min(derivative.x, 1.0);

    vec4 color = vec4(lineColor.rbg, lineColor.a - min(line, 1.0));
    if (!drawAxis)
    return color;

    // z axis
    if (pos.x > -1.0 * minimumx && pos.x < 1.0 * minimumx)
    color.xyz = vec3(0.0, 0.0, 1.0);
    // x axis
    if (pos.z > -1.0 * minimumz && pos.z < 1.0 * minimumz)
    color.xyz = vec3(1.0, 0.0, 0.0);
    return color;
}

float getLinearDepth(vec3 pos, float fallOff)
{
    float near = vsIn.camNear;
    float far = vsIn.camFar / fallOff;
    vec4 clipPos = vsIn.viewProj * vec4(pos, 1.0);
    float depth = clamp((clipPos.z / clipPos.w) * 2.0 - 1.0, -1.0, 1.0);
    depth = (2.0 * near * far) / (far + near - depth * (far - near));
    return depth / far;
}

void main() {
    float t = -vsIn.nearPos.y / (vsIn.farPos.y - vsIn.nearPos.y);
    vec3 pos = vsIn.nearPos + t * (vsIn.farPos - vsIn.nearPos);

    vec4 mainGrid = grid(pos, vec4(vec3(0.2), 0.75), 1.0, true);
    vec4 fineGrid = grid(pos, vec4(vec3(0.1), 0.5), 5.0, false);

    // Make fine grid visible only when camera is close
    float camCloseThreshold = 15.0;
    float camDist = length(vsIn.camPos - pos);
    camDist = clamp(camDist, 0.0, camCloseThreshold);
    camDist  = camDist / camCloseThreshold;
    fineGrid.a = mix(0.0, fineGrid.a, abs(1.0 - camDist));

    // Mix Main grid and fine grid
    vec3 color = mainGrid.rbg + fineGrid.rgb;
    float alpha = max(mainGrid.a, fineGrid.a);

    // Fade grid by linear depth
    alpha *= max(0.0, (0.5 - getLinearDepth(pos, 1.0)));

    outFragColor = vec4(color, alpha) * float(t > 0.0);
}