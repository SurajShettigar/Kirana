#version 460

#extension GL_GOOGLE_include_directive: enable
#extension GL_EXT_buffer_reference2: enable

#include "base_raster.glsl"

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
};

layout (set = 0, binding = 1) uniform _WorldData {
    WorldData w;
} worldBuffer;

layout (location = 0) out vec4 outFragColor;

layout (location = 0) in _VSIn {
    vec3 worldPosition;
    vec3 worldNormal;
    vec3 viewDirection;
    PrincipledData matData;
} VSIn;

// Based on Disney BRDF.
// Refer: https://media.disneyanimation.com/uploads/production/publication_asset/48/asset/s2012_pbs_disney_brdf_notes_v3.pdf
// Refer: https://google.github.io/filament/Filament.html

// Normal Distribution Function (GGX/GTR Method). Normal Distribution function estimates the distribution of
// micro-facets whose normals are facing in halfVector direction. Only these micro-facets are visible in the view direciton.
float NDF_GGX(float NoH, float roughness)
{
    float a2 = roughness * roughness;
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
    return 1.0 / PI;
}


vec3 principledBRDF(in PrincipledData matData, vec3 lightDirection, vec3 normal, vec3 viewDirection)
{
    vec3 halfVector = normalize(lightDirection + viewDirection);
    float NoH = clamp(dot(normal, halfVector), 0.0, 1.0);
    float NoV = clamp(dot(normal, viewDirection), 0.0, 1.0);
    float NoL = clamp(dot(normal, lightDirection), 0.0, 1.0);
    float LoH = clamp(dot(lightDirection, halfVector), 0.0, 1.0);

    float roughness = matData.roughness * matData.roughness;
    vec3 f0 = 0.16 * matData.specular * matData.specular * (1.0 - matData.metallic) + matData.color.rgb * matData.metallic;
    float f90 = 1.0;

    vec3 diffuseColor = (1.0 - matData.metallic) * matData.color.rgb;

    vec3 f_specular = F_Schlick(LoH, f0, f90) * V_SmithGGXCorrelated(NoV, NoL, roughness) * NDF_GGX(NoH, roughness);
    vec3 f_diffuse = diffuseColor * F_Lambert();

    return f_diffuse + f_specular;
}

void main() {
    vec3 lightDir = -normalize(worldBuffer.w.sunDirection);
    vec3 viewDir = normalize(VSIn.viewDirection);
    vec3 brdf = principledBRDF(VSIn.matData, lightDir, VSIn.worldNormal, viewDir);
    vec3 light = worldBuffer.w.sunColor.rgb * worldBuffer.w.sunIntensity * max(0.0, dot(VSIn.worldNormal, lightDir));
    vec3 luminance = brdf * light;
    outFragColor = vec4(luminance, 1.0f);
}