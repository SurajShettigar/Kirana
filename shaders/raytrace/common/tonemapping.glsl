const float GAMMA = 2.2;
const float ONE_BY_GAMMA = 1.0 / GAMMA;

vec3 linearTosRGB(in vec3 linearColor)
{
    return pow(linearColor, vec3(ONE_BY_GAMMA));
}

vec3 sRGBToLinear(in vec3 sRGBColor)
{
    return pow(sRGBColor, vec3(GAMMA));
}
