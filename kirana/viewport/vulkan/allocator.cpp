#include "allocator.hpp"

#include "instance.hpp"
#include "device.hpp"
#include "command_pool.hpp"
#include "command_buffers.hpp"

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.hpp>
#include "vulkan_utils.hpp"
#include "vulkan_types.hpp"

#include <scene.hpp>
#include <utility>

void kirana::viewport::vulkan::Allocator::displayMemoryInfo()
{
    const double MBMultiplier = 9.54e-7;

    auto budgets = m_current->getHeapBudgets();
    Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::debug,
                      "Allocator Budgets: " + std::to_string(budgets.size()));
    for (uint32_t i = 0; i < budgets.size(); i++)
    {
        const double budget =
            static_cast<double>(budgets[i].budget) * MBMultiplier;
        const double usage =
            static_cast<double>(budgets[i].usage) * MBMultiplier;
        std::string info =
            "Budget: " + std::to_string(static_cast<uint32_t>(budget)) + "MB";
        info +=
            " Usage: " + std::to_string(static_cast<uint32_t>(usage)) + "MB";
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::debug,
                          "Allocator [" + std::to_string(i) + "]" + ": " +
                              info);
    }
}

kirana::viewport::vulkan::Allocator::Allocator(const Instance *instance,
                                               const Device *device)
    : m_isInitialized{false}, m_instance{instance}, m_device{device}
{
    try
    {
        vma::AllocatorCreateInfo createInfo(
            {vma::AllocatorCreateFlagBits::eBufferDeviceAddress |
             vma::AllocatorCreateFlagBits::eExtMemoryBudget},
            m_device->gpu, m_device->current);
        createInfo.setInstance(m_instance->current);
        m_current =
            std::make_unique<vma::Allocator>(vma::createAllocator(createInfo));

        m_commandFence = m_device->current.createFence(
            vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
        m_commandPool =
            new CommandPool(m_device, m_device->queueFamilyIndices.graphics);
        m_commandPool->allocateCommandBuffers(m_commandBuffers);

        displayMemoryInfo();
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
    VK_HANDLE_RESULT(
        m_device->current.waitForFences(m_commandFence, true,
                                        constants::VULKAN_FRAME_SYNC_TIMEOUT),
        "Failed to wait for copy fence")

    if (m_commandPool)
    {
        delete m_commandPool;
        m_commandPool = nullptr;
    }
    if (m_commandBuffers)
    {
        delete m_commandBuffers;
        m_commandBuffers = nullptr;
    }
    if (m_device && m_commandFence)
        m_device->current.destroyFence(m_commandFence);
    if (m_current)
        m_current->destroy();

    Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                      "Allocator destroyed");
}

void kirana::viewport::vulkan::Allocator::setCurrentFrameIndex(
    uint32_t frameIndex)
{
    m_current->setCurrentFrameIndex(frameIndex);
}

bool kirana::viewport::vulkan::Allocator::allocateBuffer(
    vk::DeviceSize size, vk::BufferUsageFlags usageFlags,
    vma::MemoryUsage memoryUsage, AllocatedBuffer *buffer,
    bool mapMemoryPointer) const
{
    const vk::BufferCreateInfo createInfo({}, size, usageFlags);
    const vma::AllocationCreateInfo allocCreateInfo(
        {},
        memoryUsage);
    try
    {
        const std::pair<vk::Buffer, vma::Allocation> data =
            m_current->createBuffer(createInfo, allocCreateInfo);
        buffer->buffer = std::make_unique<vk::Buffer>(data.first);
        buffer->allocation = std::make_unique<vma::Allocation>(data.second);
        if (mapMemoryPointer)
            return m_current->mapMemory(*buffer->allocation,
                                        &buffer->memoryPointer) ==
                   vk::Result::eSuccess;
        return true;
    }
    catch (...)
    {
        handleVulkanException();
    }
    return false;
}

bool kirana::viewport::vulkan::Allocator::allocateBufferToGPU(
    vk::DeviceSize size, vk::BufferUsageFlags usageFlags,
    AllocatedBuffer *buffer, const void *data) const
{
    bool status = false;
    AllocatedBuffer stagingBuffer;
    status =
        allocateBuffer(size, usageFlags | vk::BufferUsageFlagBits::eTransferSrc,
                       vma::MemoryUsage::eCpuToGpu, &stagingBuffer, false);
    if (status)
        status = copyDataToMemory(stagingBuffer, size, 0, data);
    if (status)
        status = allocateBuffer(
            size, usageFlags | vk::BufferUsageFlagBits::eTransferDst,
            vma::MemoryUsage::eGpuOnly, buffer, false);
    if (status)
        status = copyBuffer(stagingBuffer, *buffer, size);
    free(stagingBuffer);
    return status;
}

bool kirana::viewport::vulkan::Allocator::allocateImage(
    const vk::ImageCreateInfo &imageCreateInfo, vk::ImageLayout layout,
    vk::ImageSubresourceRange subresourceRange, vma::MemoryUsage memoryUsage,
    vk::MemoryPropertyFlags requiredFlags, AllocateImage *image) const
{
    vma::AllocationCreateInfo allocCreateInfo({}, memoryUsage, requiredFlags);
    try
    {
        std::pair<vk::Image, vma::Allocation> data =
            m_current->createImage(imageCreateInfo, allocCreateInfo);
        image->image = std::make_unique<vk::Image>(data.first);
        image->allocation = std::make_unique<vma::Allocation>(data.second);

        // TODO: Add a better way to transition image layouts.
        // Transition the image layout from undefined.
        VK_HANDLE_RESULT(m_device->current.waitForFences(
                             m_commandFence, true,
                             constants::VULKAN_COPY_BUFFER_WAIT_TIMEOUT),
                         "Failed to wait for copy fence")
        m_device->current.resetFences(m_commandFence);
        m_commandPool->reset();
        m_commandBuffers->begin();
        m_commandBuffers->createImageMemoryBarrier(
            vk::PipelineStageFlagBits::eAllCommands,
            vk::PipelineStageFlagBits::eAllCommands, {},
            vk::ImageLayout::eUndefined, layout, *image->image,
            subresourceRange);
        m_commandBuffers->end();
        m_device->graphicsSubmit(m_commandBuffers->current[0], m_commandFence);
        VK_HANDLE_RESULT(m_device->current.waitForFences(
                             m_commandFence, true,
                             constants::VULKAN_COPY_BUFFER_WAIT_TIMEOUT),
                         "Failed to wait for copy fence")

        return true;
    }
    catch (...)
    {
        handleVulkanException();
    }
    return false;
}

bool kirana::viewport::vulkan::Allocator::copyDataToMemory(
    AllocatedBuffer &buffer, size_t size, uint32_t offset,
    const void *data) const
{
    try
    {
        if (buffer.memoryPointer == nullptr)
        {
            if (m_current->mapMemory(*buffer.allocation,
                                     &buffer.memoryPointer) !=
                vk::Result::eSuccess)
            {
                Logger::get().log(constants::LOG_CHANNEL_VULKAN,
                                  LogSeverity::error, "Failed to map memory");
                return false;
            }
        }
        // We cast it to char pointer so that we can increment the pointer by
        // offset and then copy value to that offset memory location.
        memcpy(reinterpret_cast<void *>(
                   reinterpret_cast<char *>(buffer.memoryPointer) + offset),
               data, size);
        return true;
    }
    catch (...)
    {
        handleVulkanException();
    }
    return false;
}

bool kirana::viewport::vulkan::Allocator::copyBuffer(
    const AllocatedBuffer &stagingBuffer, const AllocatedBuffer &destBuffer,
    vk::DeviceSize size, vk::DeviceSize srcOffset,
    vk::DeviceSize dstOffset) const
{
    VK_HANDLE_RESULT(
        m_device->current.waitForFences(
            m_commandFence, true, constants::VULKAN_COPY_BUFFER_WAIT_TIMEOUT),
        "Failed to wait for copy fence")
    m_device->current.resetFences(m_commandFence);
    m_commandPool->reset();
    m_commandBuffers->begin();
    vk::BufferCopy region(srcOffset, dstOffset, size);
    m_commandBuffers->copyBuffer(*stagingBuffer.buffer, *destBuffer.buffer,
                                 {region});
    m_commandBuffers->end();
    m_device->graphicsSubmit(m_commandBuffers->current[0], m_commandFence);

    VK_HANDLE_RESULT(
        m_device->current.waitForFences(
            m_commandFence, true, constants::VULKAN_COPY_BUFFER_WAIT_TIMEOUT),
        "Failed to wait for copy fence")
    return true;
}

void kirana::viewport::vulkan::Allocator::free(
    const AllocatedBuffer &buffer) const
{
    if (buffer.memoryPointer != nullptr)
        m_current->unmapMemory(*buffer.allocation);
    if (buffer.buffer)
        m_current->destroyBuffer(*buffer.buffer, *buffer.allocation);
}

void kirana::viewport::vulkan::Allocator::free(const AllocateImage &image) const
{
    if (image.image)
        m_current->destroyImage(*image.image, *image.allocation);
}