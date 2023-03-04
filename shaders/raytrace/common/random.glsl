#ifndef RAYTRACE_COMMON_RANDOM_GLSL
#define RAYTRACE_COMMON_RANDOM_GLSL 1

// Taken from Nvidia Raytracing Samples
// Link: https://github.com/nvpro-samples/vk_raytrace/blob/master/shaders/random.glsl

// Tiny Encryption Algorithm
// Paper: GPU Random Numbers via the Tiny Encryption Algorithm
// Authors: Fahad Zafar, Marc Olano, Aaron Curtis
// Link: https://redirect.cs.umbc.edu/~olano/papers/GPUTEA.pdf
uint seed(uint value0, uint value1)
{
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


// Taken from Numerical Recipes: Linear Congruential Generator
uint lcg(inout uint state)
{
    const uint LCG_A = 1664525u;
    const uint LCG_C = 1013904223u;
    state = (LCG_A * state + LCG_C);
    return state & 0x00FFFFFF;
}

// Good statistical quality, fast RNG
// Link:  https://www.pcg-random.org/
uint pcg(inout uint state)
{
    uint prev = state * 747796405u + 2891336453u;
    uint word = ((prev >> ((prev >> 28u) + 4u)) ^ prev) * 277803737u;
    state = prev;
    return (word >> 22u) ^ word;
}

// Generate a random float in the range [0, 1)
float random(inout uint seed)
{
    //    return float(lcg(seed)) / float(0x01000000);
    return uintBitsToFloat(0x3f800000 | (pcg(seed) >> 9)) - 1.0f;
}

vec2 randomVec2(inout uint seed)
{
    return vec2(random(seed), random(seed));
}

#endif