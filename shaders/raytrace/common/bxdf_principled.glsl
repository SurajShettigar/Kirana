#ifndef RAYTRACE_COMMON_BXDF_PRINCIPLED_GLSL
#define RAYTRACE_COMMON_BXDF_PRINCIPLED_GLSL 1

#include "sampling.glsl"

struct PrincipledData {
    vec4 color;
    float subSurface;
    float metallic;
    float specular;
    float specularTint;
    float roughness;
    float anisotropic;
    float sheen;
    float sheenTint;
    float clearCoat;
    float clearCoatGloss;
    float transmission;
    float ior;
    vec4 emissiveColor;
    float emissiveIntensity;
    int baseMap;
    int emissiveMap;
    int normalMap;
};

layout (buffer_reference) readonly buffer MaterialData {
    PrincipledData p[];
};

layout(set = 1, binding = 0) uniform sampler2D matTextures[];

// Based on Disney BRDF.
// Refer: https://media.disneyanimation.com/uploads/production/publication_asset/48/asset/s2012_pbs_disney_brdf_notes_v3.pdf
// Refer: https://google.github.io/filament/Filament.html

// Normal Distribution Function (GGX/GTR2 Method). Normal Distribution function estimates the distribution of
// micro-facets whose normals are facing in halfVector direction. Only these micro-facets are visible in the view direciton.
float NDF_GGX(float NoH, float roughness)
{
    float a2 = roughness * roughness;
    float denom = ((NoH * NoH * (a2 - 1.0)) + 1.0);
    return a2 / (PI * denom * denom);
}

// Importance sampling from the GGX/GTR2 normal distribution function. Gives the importance sampled half vector.
vec3 sampleNDF_GGX(in float roughness, in vec2 random, in vec3[3] transformFrame)
{
    const float a2 = roughness * roughness;
    const float phi = TWO_PI * random.x;
    const float cosTheta = sqrt((1.0 - random.y) / (1 + (a2 - 1.0) * random.y));
    const float sinThetha = clamp(sqrt(1.0 - cosTheta * cosTheta), 0.0, 1.0);

    vec3 dir = vec3(sinThetha * cos(phi), sinThetha * sin(phi), cosTheta);
    dir = dir.x * transformFrame[0] + dir.y * transformFrame[1] + dir.z * transformFrame[2];
    return dir;
}

// Visibility Function derived from Smith GGX Geometry function. Accounts for the shadows and masking from the micro-facets.
// Even the height of the micro-facets are taken into account.
float V_SmithGGXCorrelated(float NoV, float NoL, float roughness)
{
    const float a2 = roughness * roughness;
    float vv = NoL * sqrt(NoV * (-NoV * a2 + NoV) + a2);
    float vl = NoV * sqrt(NoL * (-NoL * a2 + NoL) + a2);
    return 0.5 / (vv + vl);
}

// Fresnel Reflectance using Schlick's approximation. Gives the fractional distribution of reflected light.
vec3 F_Schlick(float LoH, vec3 f0, float f90)
{
    return f0 + (vec3(f90) - f0) * pow(1.0 - LoH, 5.0);
}

// Lambertian diffuse light distribution.
float F_Lambert()
{
    return ONE_BY_PI;
}

vec3 evaluateDiffuse(inout uint seed, in IntersectionData intersection, in PrincipledData matData, in vec3 viewDir, in vec3 lightDir, inout float pdf)
{
    vec3 ffnormal = dot(viewDir, intersection.normal) < 0.0 ? - intersection.normal : intersection.normal;

    if(dot(ffnormal, lightDir) < 0.0)
        return vec3(0.0);

    vec3 diffuseColor = (1.0 - matData.metallic) * matData.color.rgb;

    if(matData.baseMap > -1)
    {
        uint matIndex = uint(matData.baseMap);
        diffuseColor *= texture(matTextures[matIndex], intersection.texCoords).rgb;
    }

    // Cos weighted diffuse pdf
    pdf = clamp(dot(ffnormal, lightDir), 0.0, 1.0) * ONE_BY_PI;
    vec3 f_diffuse = diffuseColor * F_Lambert();
    return f_diffuse;
}


vec3 evaluateSpecular(inout uint seed, in IntersectionData intersection, in PrincipledData matData, in vec3 viewDir, in vec3 lightDir, in vec3 halfVec, inout float pdf)
{
    vec3 ffnormal = dot(viewDir, intersection.normal) < 0.0 ? - intersection.normal : intersection.normal;

    if(dot(ffnormal, lightDir) < 0.0)
            return vec3(0.0);

    float roughness = max(matData.roughness * matData.roughness, 0.001);
    vec3 f0 = 0.16 * matData.specular * matData.specular * (1.0 - matData.metallic) + matData.color.rgb * matData.metallic;
    float f90 = 1.0;

    float NoH = clamp(dot(ffnormal, halfVec), 0.0, 1.0);
    float NoV = clamp(dot(ffnormal, viewDir), 0.0, 1.0);
    float NoL = clamp(dot(ffnormal, lightDir), 0.0, 1.0);
    float LoH = clamp(dot(lightDir, halfVec), 0.0, 1.0);

    float D = NDF_GGX(NoH, roughness);
    vec3 f_specular = F_Schlick(LoH, f0, f90) * V_SmithGGXCorrelated(NoV, NoL, roughness) * D;

    pdf = D * NoH / (4.0 * clamp(dot(lightDir, halfVec), 0.0, 1.0));
    return f_specular;
}


vec3 evaluateBXDF(inout uint seed, in IntersectionData intersection, in vec3 viewDir, in vec3 lightDir, inout vec3 emission, inout float pdf)
{
    PrincipledData matData = MaterialData(intersection.materialBufferAddress).p[intersection.materialIndex];
    float diffuseRatio = 0.5 * (1.0 - matData.metallic);
//    float diffuseRatio = 1.0;

    emission = matData.emissiveColor.rgb * matData.emissiveIntensity;
    if (random(seed) < diffuseRatio)
    {
        vec3 f = evaluateDiffuse(seed, intersection, matData, viewDir, lightDir, pdf);
        pdf *= diffuseRatio;
        return f;
    }
    else
    {
        vec3 halfVec = normalize(lightDir + viewDir);
        if (dot(intersection.normal, halfVec) < 0.0)
        halfVec = -halfVec;
        vec3 f = evaluateSpecular(seed, intersection, matData, viewDir, lightDir, halfVec, pdf);
        pdf *= (1.0 - diffuseRatio);
        return f;
    }
}

vec3 sampleBXDF(inout uint seed, in IntersectionData intersection, in vec3 viewDir, inout vec3 lightDir, inout vec3 emission, inout float pdf)
{
    pdf = 0.0;
    PrincipledData matData = MaterialData(intersection.materialBufferAddress).p[intersection.materialIndex];

    vec3 ffnormal = dot(viewDir, intersection.normal) < 0.0 ? - intersection.normal : intersection.normal;
    float diffuseRatio = 0.5 * (1.0 - matData.metallic);
//    float diffuseRatio = 1.0;
    emission = matData.emissiveColor.rgb * matData.emissiveIntensity;
    if (random(seed) < diffuseRatio)
    {
        lightDir = cosineSampleHemisphere(randomVec2(seed), vec3[3](intersection.tangent, intersection.bitangent, ffnormal), pdf);
        vec3 f = evaluateDiffuse(seed, intersection, matData, viewDir, lightDir, pdf);
        pdf *= diffuseRatio;
        return f;
    }
    else
    {
        float roughness = max(matData.roughness * matData.roughness, 0.001);
        vec3 halfVec = sampleNDF_GGX(roughness, randomVec2(seed), vec3[3](intersection.tangent, intersection.bitangent, ffnormal));
        lightDir = normalize(reflect(-viewDir, halfVec));
        vec3 f = evaluateSpecular(seed, intersection, matData, viewDir, lightDir, halfVec, pdf);
        pdf *= (1.0 - diffuseRatio);
        return f;
    }
    return evaluateBXDF(seed, intersection, viewDir, lightDir, emission, pdf);
}

#endif