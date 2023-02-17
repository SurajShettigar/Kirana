vec3 samplePixel(in vec2 pixelCoords, in vec2 resolution, in uint maxDepth,
in CameraData camera)
{
    // Get UV coordinates, normalized to value 0-1
    const vec2 pixCenter = pixelCoords + vec2(random(_globalPayload.seed), random(_globalPayload.seed));
    const vec2 uv = (pixCenter / resolution);

    // Convert the UV-coordinates to -1 to 1 range to calculate direction
    const vec2 ndc = uv * 2.0 - 1.0;

    Ray ray = Ray(camera.position, normalize(vec4(ndc.x, ndc.y, 1.0, 1.0) * camera.invViewProj).xyz);

    uint rayFlags = gl_RayFlagsOpaqueEXT;
    float tMin = 0.0f;
    float tMax = camera.farPlane;

    _globalPayload.ray = ray;
    _globalPayload.color = vec3(0.0);
    _globalPayload.weight = vec3(0.0);
    _globalPayload.depth = 0;

    vec3 luminance = vec3(0.0);
    vec3 currWeight = vec3(1.0);
    for (; _globalPayload.depth < 2; _globalPayload.depth++) {
        traceRayEXT(_globalAccelStruct, // Acceleration Structure
        rayFlags, // Ray flags
        0xFF, // Culling Mask. ANDed with mask set in acceleration structure.
        0, // SBT Record offset
        0, // SBT Record stride
        0, // Miss Shader Index
        _globalPayload.ray.origin, // Ray origin
        tMin, // Min Ray hit distance
        _globalPayload.ray.direction, // Ray direction
        tMax, // Max Ray hit distance
        0 // Payload location
        );

        luminance.rgb += _globalPayload.color.rgb * currWeight;
        currWeight *= _globalPayload.weight;
    }
    return luminance;
}