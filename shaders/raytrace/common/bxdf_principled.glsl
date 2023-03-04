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
};

layout (buffer_reference) readonly buffer MaterialData {
    PrincipledData p[];
};

// Based on Disney BRDF.
// Refer: https://media.disneyanimation.com/uploads/production/publication_asset/48/asset/s2012_pbs_disney_brdf_notes_v3.pdf
// Refer: https://google.github.io/filament/Filament.html

// Normal Distribution Function (GGX/GTR Method). Normal Distribution function estimates the distribution of
// micro-facets whose normals are facing in halfVector direction. Only these micro-facets are visible in the view direciton.
float NDF_GGX(float NoH, float roughness)
{
    float a2 = max(0.001, roughness * roughness);
    float denom = ((NoH * NoH * (a2 - 1.0)) + 1.0);
    return a2 / (PI * denom * denom);
}

// Visibility Function derived from Smith GGX Geometry function. Accounts for the shadows and masking from the micro-facets.
// Even the height of the micro-facets are taken into account.
float V_SmithGGXCorrelated(float NoV, float NoL, float roughness)
{
    float a2 = roughness * roughness;
    float vv = NoL * sqrt(NoV * NoV * (1.0 - a2) + a2);
    float vl = NoV * sqrt(NoL * NoL * (1.0 - a2) + a2);
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

vec3 ImportanceSampleGTR1(float rgh, float r1, float r2)
{
    float a = max(0.001, rgh);
    float a2 = a * a;

    float phi = r1 * 2.0 * PI;

    float cosTheta = sqrt((1.0 - pow(a2, 1.0 - r1)) / (1.0 - a2));
    float sinTheta = clamp(sqrt(1.0 - (cosTheta * cosTheta)), 0.0, 1.0);
    float sinPhi = sin(phi);
    float cosPhi = cos(phi);

    return vec3(sinTheta * cosPhi, sinTheta * sinPhi, cosTheta);
}

vec3 sampleBXDF(inout uint seed, in IntersectionData intersection, in vec3 viewDir, inout vec3 lightDir, inout vec3 emission, inout float pdf)
{
    PrincipledData matData = MaterialData(intersection.materialBufferAddress).p[intersection.materialIndex];

    emission = matData.emissiveColor.rgb * matData.emissiveIntensity;

    vec3 ffnormal = dot(viewDir, intersection.normal) < 0.0 ? - intersection.normal : intersection.normal;
    vec3 diffuseColor = (1.0 - matData.metallic) * matData.color.rgb;

    float diffuseRatio = 0.5 * (1.0 - matData.metallic);
    vec3 f_diffuse = diffuseColor * F_Lambert();
    //    if (random(payload.seed) < diffuseRatio)
    //    {
    lightDir = cosineSampleHemisphere(randomVec2(seed), vec3[3](intersection.tangent, intersection.bitangent, ffnormal), pdf);
    return f_diffuse;
    //    }
    //    else
    //    {
    //        float roughness = matData.roughness * matData.roughness;
    //        vec3 f0 = 0.16 * matData.specular * matData.specular * (1.0 - matData.metallic) + matData.color.rgb * matData.metallic;
    //        float f90 = 1.0;
    //        vec3 refDir = ImportanceSampleGTR1(roughness, random(payload.seed), random(payload.seed));
    //        refDir = hitpoint.tangent * refDir.x + hitpoint.bitangent * refDir.y + ffnormal * refDir.z;
    //        refDir = normalize(reflect(-viewDirection, refDir));
    //
    //        lightDirection = refDir;
    //
    //        vec3 halfVector = normalize(lightDirection + viewDirection);
    //        float NoH = clamp(dot(ffnormal, halfVector), 0.0, 1.0);
    //        float NoV = clamp(dot(ffnormal, viewDirection), 0.0, 1.0);
    //        float NoL = clamp(dot(ffnormal, lightDirection), 0.0, 1.0);
    //        float LoH = clamp(dot(lightDirection, halfVector), 0.0, 1.0);
    //
    //        float D = NDF_GGX(NoH, roughness);
    //        vec3 f_specular = F_Schlick(LoH, f0, f90) * V_SmithGGXCorrelated(NoV, NoL, roughness) * D;
    //
    //        // TODO: Add specular PDF
    //        pdf = D * 1.0 / 4.0 * PI;
    //        pdf *= (1.0 - diffuseRatio);
    //        return f_specular;
    //    }
}

#endif