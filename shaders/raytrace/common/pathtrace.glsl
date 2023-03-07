#ifndef RAYTRACE_COMMON_PATHTRACE_GLSL
#define RAYTRACE_COMMON_PATHTRACE_GLSL 1

#include "utils.glsl"
#include "random.glsl"
#include "intersection.glsl"

#if CURRENT_SHADING_TYPE == SHADING_TYPE_BASIC
#include "bxdf_basic.glsl"
#elif CURRENT_SHADING_TYPE == SHADING_TYPE_PRINCIPLED
#include "bxdf_principled.glsl"
#endif

// The file needs to be included in Ray-Gen shader. Make sure Top-Level Acceleration structure and
// payload data is defined before including this file.
// TopLevelAS variable name: _globalAccelStruct
// Payload variable name: _globalPayload

// Traces ray onto the scene and when hit, the hit information of the closest hit is recorded.
void closestHit(in Ray ray)
{
    uint rayFlags = gl_RayFlagsOpaqueEXT | gl_RayFlagsCullBackFacingTrianglesEXT;

    _globalPayload.hitDistance = INFINITY;

    traceRayEXT(_globalAccelStruct, // Acceleration Structure
    rayFlags, // Ray flags
    0xFF, // Culling Mask. ANDed with mask set in acceleration structure.
    0, // SBT Record offset
    0, // SBT Record stride
    0, // Miss Shader Index
    ray.origin, // Ray origin
    0.0, // Min Ray hit distance
    ray.direction, // Ray direction
    INFINITY, // Max Ray hit distance
    0 // Payload location
    );
}

// Used to check if a ray hits anything within max distance. Does not call closest hit shader.
// Used for shadow rays.
bool isHit(in Ray ray, float maxDistance)
{
    uint rayFlags = gl_RayFlagsTerminateOnFirstHitEXT
    | gl_RayFlagsSkipClosestHitShaderEXT
    | gl_RayFlagsCullBackFacingTrianglesEXT;

    _globalPayload.hitDistance = 0.0;

    traceRayEXT(_globalAccelStruct, // Acceleration Structure
    rayFlags, // Ray flags
    0xFF, // Culling Mask. ANDed with mask set in acceleration structure.
    0, // SBT Record offset
    0, // SBT Record stride
    0, // Miss Shader Index
    ray.origin, // Ray origin
    0.0, // Min Ray hit distance
    ray.direction, // Ray direction
    maxDistance, // Max Ray hit distance
    0 // Payload location
    );

    return _globalPayload.hitDistance < maxDistance;
}

vec3 samplePixel(in vec2 pixelCoords,
in vec2 resolution,
in CameraData camera,
in WorldData worldData,
in PathtraceParameters params)
{
    vec3 radiance = vec3(0.0);
    vec3 throughput = vec3(1.0);

    // Sample within each pixel for anti-aliasing
    const vec2 pixSample = randomVec2(_globalPayload.seed);
    const vec2 pixCenter = pixelCoords + pixSample;
    // Get screen coordinates, normalized to value [0, 1]
    const vec2 screenCoords = (pixCenter / resolution);
    // Convert screen coords. to Normalized Device Coordinates in the range [-1, 1]
    const vec2 ndc = screenCoords * 2.0 - 1.0;

    // Convert from NDC to world-space direction
    const vec3 rayDirection = normalize(vec4(ndc.x, ndc.y, 1.0, 1.0) * camera.invViewProj).xyz;
    Ray ray = Ray(camera.position, rayDirection);

    // Pathtracing algorithm
    for (uint i = 0; i < params.maxDepth; i++)
    {
        closestHit(ray);
        // If ray hit the environment
        if (_globalPayload.hitDistance == INFINITY)
        {
            // TODO: Sample IBL environmnet
            #if CURRENT_SHADING_TYPE == SHADING_TYPE_PRINCIPLED
            float yPos = 1.0 - float(gl_LaunchIDEXT.y) / float(gl_LaunchSizeEXT.y);
            vec3 envRadiance = mix(vec3(1.0), vec3(0.0, 0.47, 0.99), yPos);
            #else
            vec3 envRadiance = worldData.ambientColor.rgb;
            #endif
            return radiance + (envRadiance * throughput) * (i == 0 ? 1.0 : 0.5);
        }
        else
        {
            IntersectionData intersection = getIntesectionData(_globalPayload);
            vec3 viewDir = - ray.direction;

            // TODO: Sample Lights
            // Indirect light contribution
            vec3 emission = vec3(0.0);
            vec3 newRayOrigin = offsetRay(intersection.position, intersection.normal);
            vec3 newRayDirection = ray.direction;
            float pdf = 0.0;

            vec3 bxdf = sampleBXDF(_globalPayload.seed, intersection, viewDir, newRayDirection, emission, pdf);
            radiance += emission * throughput;

            if (pdf < EPSILON)
            break;

            float NoL = abs(dot(intersection.normal, newRayDirection));

            // Direct light contribution
            // Sun light contribution
            const vec3 lightDir = normalize(- worldData.sunDirection);
            Ray shadowRay = Ray(newRayOrigin, lightDir);
            if (!isHit(shadowRay, 100.0))
            {
                const float NoL = abs(dot(intersection.normal, lightDir));
                const vec3 lightIntensity = worldData.sunIntensity * worldData.sunColor.rgb * 2.0;
                const float lightPdf = 1.0;
                vec3 f_emm = vec3(0.0);
                float f_pdf = 0.0;
                radiance += evaluateBXDF(_globalPayload.seed, intersection, viewDir, lightDir, f_emm, f_pdf) * lightIntensity * NoL / lightPdf * throughput;
            }

            throughput *= bxdf * NoL / pdf;

            ray = Ray(newRayOrigin, newRayDirection);
        }
    }

    return radiance;
}

#endif