#include "depth_buffer.hpp"
#include "device.hpp"
#include "allocator.hpp"
#include "vulkan_utils.hpp"
#include <vk_mem_alloc.hpp>

kirana::viewport::vulkan::DepthBuffer::DepthBuffer(
    const Device *const device, const Allocator *const allocator,
    const std::array<uint32_t, 2> &windowResolution)
    : m_isInitialized{false}, m_format{vk::Format::eD32Sfloat},
      m_device{device}, m_allocator{allocator}
{
    vk::Extent3D extent(windowResolution[0], windowResolution[1], 1);

    vk::ImageCreateInfo imgCreateInfo(
        {}, vk::ImageType::e2D, m_format, extent, 1, 1,
        vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eDepthStencilAttachment);

    if (m_allocator->allocateImage(imgCreateInfo, vma::MemoryUsage::eGpuOnly,
                                   vk::MemoryPropertyFlagBits::eDeviceLocal,
                                   &m_image))
    {
        vk::ImageSubresourceRange subRR(vk::ImageAspectFlagBits::eDepth, 0, 1,
                                        0, 1);
        vk::ImageViewCreateInfo imgViewCreateInfo(
            {}, *(m_image.image), vk::ImageViewType::e2D, m_format,
            vk::ComponentMapping(), subRR);

        try
        {
            m_imageView = m_device->current.createImageView(imgViewCreateInfo);
            m_isInitialized = true;
            Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                              "Depth Buffer created");
        }
        catch (...)
        {
            handleVulkanException();
        }
    }
}

kirana::viewport::vulkan::DepthBuffer::~DepthBuffer()
{
    if (m_device)
    {
        if (m_imageView)
            m_device->current.destroyImageView(m_imageView);
        if (m_image.image)
            m_allocator->free(m_image);
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                          "Depth Buffer destroyed");
    }
}