#include "texture.hpp"

#include "device.hpp"
#include <vk_mem_alloc.hpp>
#include "allocator.hpp"
#include "texture_sampler.hpp"
#include "vulkan_utils.hpp"

#include <utility>


bool kirana::viewport::vulkan::Texture::createImageView()
{
    const vk::ImageViewCreateInfo imgViewCreateInfo(
        {}, m_image, m_properties.imageViewType, m_properties.format,
        vk::ComponentMapping(), m_subresourceRange);

    try
    {
        m_imageView = m_device->current.createImageView(imgViewCreateInfo);
        m_device->setDebugObjectName(m_imageView, "ImageView_" + m_name);
        m_descInfo = vk::DescriptorImageInfo{
            m_sampler == nullptr ? nullptr : m_sampler->current, m_imageView,
            m_properties.layout};
        return true;
    }
    catch (...)
    {
        handleVulkanException();
    }
    return false;
}

kirana::viewport::vulkan::Texture::Texture(
    const Device *const device, const Allocator *const allocator,
    const Properties &properties, const TextureSampler *const sampler,
    std::string name, const void *pixelData, size_t pixelDataSize)
    : m_isInitialized{false}, m_device{device}, m_allocator{allocator},
      m_properties{properties}, m_sampler{sampler}, m_name{std::move(name)}
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
        &m_allocatedImage, imgCreateInfo, m_properties.layout,
        m_subresourceRange,
        pixelData == nullptr ? Allocator::AllocationType::GPU_READ_ONLY
                             : Allocator::AllocationType::GPU_WRITEABLE,
        pixelData, pixelDataSize, {0, 0, 0}, m_properties.size);
    if (allocated)
    {
        m_image = *m_allocatedImage.image;
        m_device->setDebugObjectName(m_image, "Image_" + m_name);
        m_isInitialized = createImageView();
        if (m_isInitialized)
        {
            Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                              "Texture created: " + m_name);
        }
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
                                           const TextureSampler *const sampler,
                                           std::string name)
    : m_isInitialized{false}, m_device{device}, m_allocator{nullptr},
      m_properties{properties}, m_sampler{sampler}, m_name{std::move(name)},
      m_image{image}
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
        if (m_allocator)
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
                               vk::ImageLayout::eDepthStencilAttachmentOptimal},
                    nullptr, "Depth_Texture");
    return depthTexture->m_isInitialized;
}