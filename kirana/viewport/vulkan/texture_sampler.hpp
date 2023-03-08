#ifndef KIRANA_VIEWPORT_VULKAN_TEXTURE_SAMPLER_HPP
#define KIRANA_VIEWPORT_VULKAN_TEXTURE_SAMPLER_HPP

#include "vulkan_types.hpp"
#include <vector4.hpp>

namespace kirana::viewport::vulkan
{

class Device;

class TextureSampler
{
  public:
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
    enum class BorderColor
    {
        FLOAT_TRANSPARENT_BLACK = 0,
        INT_TRANSPARENT_BLACK = 1,
        FLOAT_OPAQUE_BLACK = 2,
        INT_OPAQUE_BLACK = 3,
        FLOAT_OPAQUE_WHITE = 4,
        INT_OPAQUE_WHITE = 5,
        FLOAT_CUSTOM = 1000287003,
        INT_CUSTOM = 1000287004,
    };
    struct Properties
    {
        Filter magFilter = Filter::LINEAR;
        Filter minFilter = Filter::LINEAR;
        MipMapFilter mipMapFilter = MipMapFilter::LINEAR;
        std::array<WrapMode, 3> uvwWrapMode = {
            WrapMode::REPEAT, WrapMode::REPEAT, WrapMode::REPEAT};
        float mipLODBias = 0.0f;
        bool enableAnisotropicFiltering = true;
        float anisotropicLevel = std::numeric_limits<float>::max();
        BorderColor borderColor = BorderColor::INT_OPAQUE_BLACK;
        math::Vector4 borderColorCustom;

        bool operator==(const Properties &props) const;
        bool operator!=(const Properties &props) const
        {
            return !(*this == props);
        }
    };

    static inline TextureSampler getDefaultSampler(const Device *const device)
    {
        return TextureSampler(device, Properties{});
    }

    static inline TextureSampler getPointSampler(const Device *const device)
    {
        return TextureSampler(
            device,
            Properties{Filter::NEAREST,
                       Filter::NEAREST,
                       MipMapFilter::NEAREST,
                       {WrapMode::REPEAT, WrapMode::REPEAT, WrapMode::REPEAT},
                       0.0f,
                       false});
    }

    TextureSampler(const Device *device, Properties properties);
    ~TextureSampler();

    TextureSampler(const TextureSampler &textureSampler) = delete;
    TextureSampler &operator=(const TextureSampler &textureSampler) = delete;

    const bool &isInitialized = m_isInitialized;
    const vk::Sampler &current = m_current;

    [[nodiscard]] inline const Properties &getProperties() const
    {
        return m_properties;
    }

  private:
    bool m_isInitialized = false;
    const Device *const m_device = nullptr;

    Properties m_properties;
    vk::Sampler m_current;
};
} // namespace kirana::viewport::vulkan
#endif