#ifndef KIRANA_SCENE_IMAGE_TYPES_HPP
#define KIRANA_SCENE_IMAGE_TYPES_HPP

#include <array>

namespace kirana::scene
{
enum class Channels
{
    NONE = 0,
    GRAYSCALE = 1,
    GRAYSCALE_ALPHA = 2,
    RGB = 3,
    RGBA = 4
};

enum class ColorSpace
{
    sRGB = 0,
    Linear = 1
};
enum class Filter
{
    NEAREST = 0,
    LINEAR = 1,
    CUBIC = 1000015000
};
enum class MipMapFilter
{
    NEAREST = 0,
    LINEAR = 1,
};
enum class WrapMode
{
    REPEAT = 0,
    MIRRORED_REPEAT = 1,
    CLAMP_TO_EDGE = 2,
    CLAMP_TO_BORDER = 3,
    MIRROR_CLAMP_TO_EDGE = 4,
};

enum class TextureMapping
{
    UV = 0,
    SPHERICAL = 1,
    CYLINDRICAL = 2,
    BOX = 3,
    PLANE = 4
};

struct ImageProperties
{
    Channels channels = Channels::RGBA;
    Channels fileChannels = Channels::RGBA;
    ColorSpace colorSpace = ColorSpace::sRGB;
    Filter magFilter = Filter::LINEAR;
    Filter minFilter = Filter::LINEAR;
    MipMapFilter mipMapFilter = MipMapFilter::LINEAR;
    std::array<WrapMode, 3> uvwWrapMode = {WrapMode::REPEAT, WrapMode::REPEAT,
                                           WrapMode::REPEAT};
    uint32_t mipLODBias = 0;
    bool enableAnisotropicFiltering = true;
    uint32_t anisotropicLevel = 16;
    uint32_t uvIndex = 0;
    TextureMapping textureMapping = TextureMapping::UV;
};
} // namespace kirana::scene

#endif