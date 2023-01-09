#include "texture.hpp"

#include "device.hpp"
#include <utility>
#include <vk_mem_alloc.hpp>
#include "allocator.hpp"
#include "vulkan_utils.hpp"


vk::Sampler kirana::viewport::vulkan::Texture::getSampler() const
{
    // TODO: Add support for different type of samplers.
    const vk::SamplerCreateInfo createInfo{
        {},
        vk::Filter::eNearest,
        vk::Filter::eNearest,
        vk::SamplerMipmapMode::eNearest,
        vk::SamplerAddressMode::eRepeat,
        vk::SamplerAddressMode::eRepeat,
        vk::SamplerAddressMode::eRepeat,
    };

    return m_device->current.createSampler(createInfo);
}

bool kirana::viewport::vulkan::Texture::createImageView()
{
    const vk::ImageViewCreateInfo imgViewCreateInfo(
        {}, m_image, m_properties.imageViewType, m_properties.format,
        vk::ComponentMapping(), m_subresourceRange);

    try
    {
        m_imageView = m_device->current.createImageView(imgViewCreateInfo);

        if (m_properties.generateDescriptorInfo)
            m_descInfo = vk::DescriptorImageInfo{getSampler(), m_imageView,
                                                 m_properties.layout};
        return true;
    }
    catch (...)
    {
        handleVulkanException();
    }
    return false;
}

kirana::viewport::vulkan::Texture::Texture(const Device *const device,
                                           const Allocator *const allocator,
                                           const Properties &properties,
                                           std::string name,
                                           const void *pixelData)
    : m_isInitialized{false}, m_device{device}, m_allocator{allocator},
      m_properties{properties}, m_name{std::move(name)}
{
    const vk::Extent3D extent(m_properties.size[0], m_properties.size[1],
                              m_properties.size[2]);

    const vk::ImageCreateInfo imgCreateInfo(
        {}, m_properties.imageType, m_properties.format, extent,
        m_properties.numMipLevels, m_properties.numLayers,
        vk::SampleCountFlagBits::e1, m_properties.tiling, m_properties.usage);

    m_subresourceRange = vk::ImageSubresourceRange(m_properties.aspect, 0,
                                                   m_properties.numMipLevels, 0,
                                                   m_properties.numLayers);
    bool allocated = false;
    // TODO: Copy Pixel data if it exits to vk::Image
    allocated = m_allocator->allocateImage(
        imgCreateInfo, m_properties.layout, m_subresourceRange,
        vma::MemoryUsage::eGpuOnly, vk::MemoryPropertyFlagBits::eDeviceLocal,
        &m_allocatedImage);
    if (allocated)
    {
        m_image = *m_allocatedImage.image;
        m_isInitialized = createImageView();
        if (m_isInitialized)
            Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                              "Texture created: " + m_name);
    }
    else
    {
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                          "Failed to allocate memory to Texture: " + m_name);
    }
}


kirana::viewport::vulkan::Texture::Texture(const Device *device,
                                           const vk::Image &image,
                                           const Properties &properties,
                                           std::string name)
    : m_isInitialized{false}, m_device{device}, m_allocator{nullptr},
      m_properties{properties}, m_name{std::move(name)}, m_image{image}
{
    m_subresourceRange = vk::ImageSubresourceRange(m_properties.aspect, 0,
                                                   m_properties.numMipLevels, 0,
                                                   m_properties.numLayers);
    m_isInitialized = createImageView();
    if (m_isInitialized)
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                          "Texture created: " + m_name);
}

kirana::viewport::vulkan::Texture::~Texture()
{
    if (m_device)
    {
        if (m_descInfo.sampler)
            m_device->current.destroySampler(m_descInfo.sampler);
        if (m_imageView)
            m_device->current.destroyImageView(m_imageView);
        if (m_allocator && m_allocatedImage.image)
            m_allocator->free(m_allocatedImage);
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                          "Texture Destroyed: " + m_name);
    }
}

bool kirana::viewport::vulkan::Texture::createDepthTexture(
    const Device *const device, const Allocator *const allocator,
    const std::array<uint32_t, 2> &windowResolution,
    const Texture *&depthTexture)
{
    depthTexture =
        new Texture(device, allocator,
                    Properties{{windowResolution[0], windowResolution[1], 1},
                               vk::Format::eD32SfloatS8Uint,
                               vk::ImageUsageFlagBits::eDepthStencilAttachment,
                               vk::ImageAspectFlagBits::eDepth |
                                   vk::ImageAspectFlagBits::eStencil,
                               false,
                               vk::ImageLayout::eDepthStencilAttachmentOptimal},
                    "Depth_Texture");
    return depthTexture->m_isInitialized;
}