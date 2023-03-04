#ifndef RAYTRACE_COMMON_SAMPLING_GLSL
#define RAYTRACE_COMMON_SAMPLING_GLSL 1


// Following sampling functions are taken from PBRT 3rd Edition.
// Link: https://pbr-book.org/3ed-2018/Monte_Carlo_Integration/2D_Sampling_with_Multidimensional_Transformations#UniformlySamplingaHemisphere

vec3 uniformSampleHemisphere(in vec2 random, in vec3[3] transformFrame, out float pdf) {
    const float z = random.x;
    const float r = sqrt(max(0.0, 1.0 - z * z));
    const float phi = TWO_PI * random.y;

    vec3 dir = vec3(r * cos(phi), r * sin(phi), z);
    dir = dir.x * transformFrame[0] + dir.y * transformFrame[1] + dir.z * transformFrame[2];

    pdf = ONE_BY_TWO_PI; // 1/2PI
    return dir;
}

vec3 uniformSampleSphere(in vec2 random, in vec3[3] transformFrame, out float pdf)
{
    const float z = 1.0 - 2.0 * random.x;
    const float r = sqrt(max(0.0, 1.0 - z * z));
    const float phi = TWO_PI * random.y;

    vec3 dir =  vec3(r * cos(phi), r * sin(phi), z);
    dir = dir.x * transformFrame[0] + dir.y * transformFrame[1] + dir.z * transformFrame[2];

    pdf = ONE_BY_FOUR_PI; // 1/4PI
    return dir;
}

vec2 uniformSampleDisk(in vec2 random)
{
    const float r = sqrt(random.x);
    const float theta = TWO_PI * random.y;
    return r * vec2(cos(theta), sin(theta));
}

vec2 concentricSampleDisk(in vec2 random)
{
    random = 2.0 * random - vec2(1.0);
    if (abs(random.x) <= EPSILON && abs(random.y) <= EPSILON)
    return vec2(0.0);

    const bool comp = abs(random.x) > abs(random.y);
    const float r = comp ? random.x : random.y;
    const float theta = comp ? PI_BY_FOUR * (random.y / random.x) : PI_BY_TWO - PI_BY_FOUR * (random.x / random.y);
    return r * vec2(cos(theta), sin(theta));
}

vec3 cosineSampleHemisphere(in vec2 random, in vec3[3] transformFrame, out float pdf) {
    vec2 d = uniformSampleDisk(random);
    float z = sqrt(max(0.0, 1.0 - d.x * d.x - d.y * d.y));

    vec3 dir = vec3(d.x, d.y, z);
    dir = dir.x * transformFrame[0] + dir.y * transformFrame[1] + dir.z * transformFrame[2];

    pdf = clamp(dot(transformFrame[2], dir), 0.0, 1.0) * ONE_BY_PI;
    return dir;
}

#endif