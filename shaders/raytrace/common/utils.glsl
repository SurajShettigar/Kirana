#ifndef RAYTRACE_COMMON_UTILS_GLSL
#define RAYTRACE_COMMON_UTILS_GLSL 1

void getCoordinateFrame_Duff(in vec3 normal, out vec3 tangent, out vec3 binormal) {
    float zSign = normal.z >= 0.0 ? 1.0 : -1.0;
    const float a = -1.0 / (zSign + normal.z);
    const float b = normal.x * normal.y * a;
    tangent = vec3(b, zSign + normal.y * normal.y * a, - normal.y);
    binormal = vec3(1.0 + zSign * normal.x * normal.x * a, zSign * b, -zSign * normal.x);
}

void getCoordinateFrame_Frisvad(in vec3 normal, out vec3 tangent, out vec3 binormal) {
    if (normal.z < -0.9999)
    {
        tangent = vec3(-1.0, 0.0, 0.0);
        binormal = vec3(0.0, -1.0, 0.0);
        return;
    }
    const float a = 1.0 / (1.0 + normal.z);
    const float b = - normal.x * normal.y * a;
    tangent = vec3(b, 1.0 - normal.y * normal.y * a, - normal.y);
    binormal = vec3(1.0 - normal.x * normal.x * a, b, - normal.x);
}

void getCoordinateFrame_Moller(in vec3 normal, out vec3 tangent, out vec3 binormal) {
    if (abs(normal.x) > abs(normal.y))
    tangent = vec3(normal.z, 0, - normal.x) / sqrt(normal.x * normal.x + normal.z * normal.z);
    else
    tangent = vec3(0, - normal.z, normal.y) / sqrt(normal.y * normal.y + normal.z * normal.z);
    binormal = cross(normal, tangent);
}

#endif