#ifndef KIRANA_TEXTURE_HPP
#define KIRANA_TEXTURE_HPP

#include "vulkan_types.hpp"

namespace kirana::viewport::vulkan
{
class Device;
class Allocator;
class Texture
{
  public:
    struct Properties
    {
        std::array<uint32_t, 3> size;
        vk::Format format;
        vk::ImageUsageFlags usage;
        vk::ImageAspectFlags aspect;
        bool generateDescriptorInfo = true;
        vk::ImageType imageType = vk::ImageType::e2D;
        vk::ImageViewType imageViewType = vk::ImageViewType::e2D;
        vk::SampleCountFlags sampleCount = vk::SampleCountFlagBits::e1;
        vk::ImageTiling tiling = vk::ImageTiling::eOptimal;
        vk::ImageLayout layout = vk::ImageLayout::eShaderReadOnlyOptimal;
        uint32_t numMipLevels = 1;
        uint32_t numLayers = 1;
    };

  private:
    bool m_isInitialized = false;
    Properties m_properties;
    std::string m_name = "Texture";
    AllocateImage m_image;
    vk::ImageView m_imageView;

    const Device *const m_device;
    const Allocator *const m_allocator;

    vk::Sampler getSampler() const;

  public:
    explicit Texture(const Device *device, const Allocator *allocator,
                     const Properties &properties, std::string name = "Texture",
                     const void *pixelData = nullptr);
    ~Texture();
    Texture(const Texture &texture) = delete;
    Texture &operator=(const Texture &texture) = delete;

    const bool &isInitialized = m_isInitialized;

    [[nodiscard]] inline const Properties &getProperties() const
    {
        return m_properties;
    }

    [[nodiscard]] inline const vk::ImageView &getImageView() const
    {
        return m_imageView;
    }

    static bool createDepthTexture(
        const Device *device, const Allocator *,
        const std::array<uint32_t, 2> &windowResolution,
        const Texture *&depthTexture);
};
} // namespace kirana::viewport::vulkan

#endif