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

vec3 offsetRay(in vec3 p, in vec3 n)
{
    const float intScale   = 256.0f;
    const float floatScale = 1.0f / 65536.0f;
    const float origin     = 1.0f / 32.0f;

    ivec3 of_i = ivec3(intScale * n.x, intScale * n.y, intScale * n.z);

    vec3 p_i = vec3(intBitsToFloat(floatBitsToInt(p.x) + ((p.x < 0) ? -of_i.x : of_i.x)),
    intBitsToFloat(floatBitsToInt(p.y) + ((p.y < 0) ? -of_i.y : of_i.y)),
    intBitsToFloat(floatBitsToInt(p.z) + ((p.z < 0) ? -of_i.z : of_i.z)));

    return vec3(abs(p.x) < origin ? p.x + floatScale * n.x : p_i.x,
    abs(p.y) < origin ? p.y + floatScale * n.y : p_i.y,  //
    abs(p.z) < origin ? p.z + floatScale * n.z : p_i.z);
}


#endif