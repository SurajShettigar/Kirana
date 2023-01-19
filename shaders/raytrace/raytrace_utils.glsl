// Refer Nvidia Raytracing Samples for the random-sampling functions

uint getSeed(uint value0, uint value1)
{
    // Tiny Encryption Algorithm
    // Paper: GPU Random Numbers via the Tiny Encryption Algorithm
    // Authors: Fahad Zafar, Marc Olano, Aaron Curtis
    // Link: https://redirect.cs.umbc.edu/~olano/papers/GPUTEA.pdf

    uint v0 = value0;
    uint v1 = value1;
    uint s0 = 0;

    for (uint n = 0; n < 16; n++)
    {
        s0 += 0x9e3779b9;
        v0 += ((v1 << 4) + 0xa341316c) ^ (v1 + s0) ^ ((v1 >> 5) + 0xc8013ea4);
        v1 += ((v0 << 4) + 0xad90777d) ^ (v0 + s0) ^ ((v0 >> 5) + 0x7e95761e);
    }

    return v0;
}


uint lcg(inout uint seed)
{
    // Taken from Numerical Recipes: Linear Congruential Generator
    const uint LCG_A = 1664525u;
    const uint LCG_C = 1013904223u;
    seed = (LCG_A * seed + LCG_C);
    return seed & 0x00FFFFFF;
}

// Generate a random float in the range [0, 1)
float random(inout uint seed)
{
    return float(lcg(seed)) / float(0x01000000);
}

vec3 randomHemispherical(inout uint seed, in vec3[3] coordinateFrame) {
    const float PI = 3.141592;
    const float rand1 = random(seed);
    const float rand2 = random(seed);
    const float sq = sqrt(1.0 - rand2);
    vec3 value = vec3(cos(2.0 * PI * rand1) * sq, sin(2.0 * PI * rand2) * sq, sqrt(rand2));
    value = value.x * coordinateFrame[0] + value.y * coordinateFrame[1] + value.z * coordinateFrame[2];
    return value;
}

void getCoordinateFrame(in vec3 normal, out vec3 tangent, out vec3 binormal) {
    if (abs(normal.x) > abs(normal.y))
    tangent = vec3(normal.z, 0, - normal.x) / sqrt(normal.x * normal.x + normal.z * normal.z);
    else
    tangent = vec3(0, - normal.z, normal.y) / sqrt(normal.y * normal.y + normal.z * normal.z);
    binormal = cross(normal, tangent);
}