#ifndef RAYTRACE_COMMON_BXDF_PRINCIPLED_GLSL
#define RAYTRACE_COMMON_BXDF_PRINCIPLED_GLSL 1

#include "sampling.glsl"

struct BasicShadedData {
    vec4 color;
};
layout (buffer_reference) readonly buffer MaterialData {
    BasicShadedData b[];
};

vec3 evaluateBXDF(inout uint seed, in IntersectionData intersection, in vec3 viewDir, in vec3 lightDir, in vec3 shadingNormal, inout vec3 emission, inout float pdf)
{
    if(dot(shadingNormal, lightDir) < 0.0)
    return vec3(0.0);

    BasicShadedData matData = MaterialData(intersection.materialBufferAddress).b[intersection.materialIndex];
    vec3 f_diffuse = matData.color.rgb * ONE_BY_PI;

    emission = vec3(0.0);
    pdf = clamp(dot(shadingNormal, lightDir), 0.0, 1.0) * ONE_BY_PI;
    return f_diffuse;
}

vec3 sampleBXDF(inout uint seed, in IntersectionData intersection, in vec3 viewDir, inout vec3 lightDir, inout vec3 shadingNormal, inout vec3 emission, inout float pdf)
{
    shadingNormal = intersection.shadingSpace[2];
    emission = vec3(0.0);
    pdf = 0.0;
    lightDir = cosineSampleHemisphere(randomVec2(seed), intersection.shadingSpace, pdf);
    return evaluateBXDF(seed, intersection, viewDir, lightDir, shadingNormal, emission, pdf);
}

#endif