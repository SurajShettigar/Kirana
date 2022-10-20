#include "allocator.hpp"

#include "instance.hpp"
#include "device.hpp"

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.hpp>
#include "vulkan_utils.hpp"
#include "vulkan_types.hpp"

#include <scene.hpp>
#include <utility>


kirana::viewport::vulkan::Allocator::Allocator(const Instance *instance,
                                               const Device *device)
    : m_isInitialized{false}, m_instance{instance}, m_device{device}
{
    try
    {
        vma::AllocatorCreateInfo createInfo({}, m_device->gpu,
                                            m_device->current);
        createInfo.setInstance(m_instance->current);
        m_current =
            std::make_unique<vma::Allocator>(vma::createAllocator(createInfo));
        m_isInitialized = true;
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                          "Allocator created");
    }
    catch (...)
    {
        handleVulkanException();
    }
}

kirana::viewport::vulkan::Allocator::~Allocator()
{
    if (m_current)
        m_current->destroy();

    Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                      "Allocator destroyed");
}

bool kirana::viewport::vulkan::Allocator::allocateBuffer(
    vk::DeviceSize size, vk::BufferUsageFlags usageFlags,
    vma::MemoryUsage memoryUsage, AllocatedBuffer *buffer) const
{
    vk::BufferCreateInfo createInfo({}, size, usageFlags);
    vma::AllocationCreateInfo allocCreateInfo({}, memoryUsage);
    try
    {
        std::pair<vk::Buffer, vma::Allocation> data =
            m_current->createBuffer(createInfo, allocCreateInfo);
        buffer->buffer = std::make_unique<vk::Buffer>(data.first);
        buffer->allocation = std::make_unique<vma::Allocation>(data.second);
        return true;
    }
    catch (...)
    {
        handleVulkanException();
    }
    return false;
}

bool kirana::viewport::vulkan::Allocator::allocateImage(
    const vk::ImageCreateInfo &imageCreateInfo, vma::MemoryUsage memoryUsage,
    vk::MemoryPropertyFlags requiredFlags, AllocateImage *image) const
{
    vma::AllocationCreateInfo allocCreateInfo({}, memoryUsage, requiredFlags);
    try
    {
        std::pair<vk::Image, vma::Allocation> data =
            m_current->createImage(imageCreateInfo, allocCreateInfo);
        image->image = std::make_unique<vk::Image>(data.first);
        image->allocation = std::make_unique<vma::Allocation>(data.second);
        return true;
    }
    catch (...)
    {
        handleVulkanException();
    }
    return false;
}


bool kirana::viewport::vulkan::Allocator::mapToMemory(
    const AllocatedBuffer &buffer, size_t size, uint32_t offset, const void *data) const
{
    try
    {
        char *temp;
        if (m_current->mapMemory(*buffer.allocation, (void**)&temp) !=
            vk::Result::eSuccess)
        {
            Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
                              "Failed to map memory");
            return false;
        }
        temp += offset;
        memcpy(temp, data, size);
        m_current->unmapMemory(*buffer.allocation);
        return true;
    }
    catch (...)
    {
        handleVulkanException();
    }
    return false;
}

void kirana::viewport::vulkan::Allocator::free(
    const AllocatedBuffer &buffer) const
{
    m_current->destroyBuffer(*buffer.buffer, *buffer.allocation);
}

void kirana::viewport::vulkan::Allocator::free(
    const AllocateImage &image) const
{
    m_current->destroyImage(*image.image, *image.allocation);
}