#ifndef RAYTRACE_COMMON_BXDF_PRINCIPLED_GLSL
#define RAYTRACE_COMMON_BXDF_PRINCIPLED_GLSL 1

#include "sampling.glsl"

struct BasicShadedData {
    vec4 color;
};
layout (buffer_reference) readonly buffer MaterialData {
    BasicShadedData b[];
};

vec3 sampleBXDF(inout uint seed, in IntersectionData intersection, in vec3 viewDir, inout vec3 lightDir, inout vec3 emission, inout float pdf)
{
    BasicShadedData matData = MaterialData(intersection.materialBufferAddress).b[intersection.materialIndex];

    emission = vec3(0.0);

    vec3 ffnormal = dot(viewDir, intersection.normal) < 0.0 ? - intersection.normal : intersection.normal;
    vec3 f_diffuse = matData.color.rgb * ONE_BY_PI;
    lightDir = cosineSampleHemisphere(randomVec2(seed), vec3[3](intersection.tangent, intersection.bitangent, ffnormal), pdf);
    return f_diffuse;
}

#endif