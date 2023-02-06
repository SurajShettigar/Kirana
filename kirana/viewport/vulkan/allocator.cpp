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
    AllocatedBuffer *buffer, vk::DeviceSize bufferSize,
    vk::BufferUsageFlags usageFlags, AllocationType allocationType,
    const void *data, size_t dataOffset, size_t dataSize) const
{
    if (data != nullptr && allocationType == AllocationType::GPU_READ_ONLY)
    {
        Logger::get().log(
            constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
            "You cannot copy data to the buffer if the allocation type is "
            "GPU_READ_ONLY. Either set it to GPU_WRITEABLE or WRITEABLE");
        return false;
    }

    vma::AllocationCreateFlags createFlags;
    switch (allocationType)
    {
    case AllocationType::GPU_READ_ONLY:
    case AllocationType::GPU_WRITEABLE:
        createFlags = vma::AllocationCreateFlagBits::eDedicatedMemory;
        break;
    case AllocationType::WRITEABLE:
        createFlags =
            vma::AllocationCreateFlagBits::eHostAccessSequentialWrite |
            vma::AllocationCreateFlagBits::eHostAccessAllowTransferInstead |
            vma::AllocationCreateFlagBits::eMapped;
        break;
    case AllocationType::READ_BACK:
        createFlags = vma::AllocationCreateFlagBits::eHostAccessRandom |
                      vma::AllocationCreateFlagBits::eMapped;
    }
    if (allocationType == AllocationType::GPU_WRITEABLE ||
        allocationType == AllocationType::WRITEABLE)
        usageFlags |= vk::BufferUsageFlagBits::eTransferDst;

    const vk::BufferCreateInfo createInfo({}, bufferSize, usageFlags);
    const vma::AllocationCreateInfo allocCreateInfo(createFlags,
                                                    vma::MemoryUsage::eAuto);
    try
    {
        const std::pair<vk::Buffer, vma::Allocation> bufferData =
            m_current->createBuffer(createInfo, allocCreateInfo);
        buffer->buffer = std::make_unique<vk::Buffer>(bufferData.first);
        buffer->allocation =
            std::make_unique<vma::Allocation>(bufferData.second);

        if (usageFlags & vk::BufferUsageFlagBits::eShaderDeviceAddress)
            buffer->address = m_device->getBufferAddress(*buffer->buffer);

        bool result = true;
        if (data != nullptr)
            result = copyDataToBuffer(*buffer, data, dataOffset, dataSize);
        return result;
    }
    catch (...)
    {
        handleVulkanException();
    }
    return false;
}

bool kirana::viewport::vulkan::Allocator::allocateImage(
    AllocateImage *image, vk::ImageCreateInfo imageCreateInfo,
    vk::ImageLayout layout, vk::ImageSubresourceRange subresourceRange,
    AllocationType allocationType, const void *data, size_t dataOffset,
    size_t dataSize) const
{
    vma::AllocationCreateFlags createFlags;
    switch (allocationType)
    {
    case AllocationType::GPU_READ_ONLY:
    case AllocationType::GPU_WRITEABLE:
        createFlags = vma::AllocationCreateFlagBits::eDedicatedMemory;
        break;
    case AllocationType::WRITEABLE:
        createFlags =
            vma::AllocationCreateFlagBits::eHostAccessSequentialWrite |
            vma::AllocationCreateFlagBits::eHostAccessAllowTransferInstead |
            vma::AllocationCreateFlagBits::eMapped;
        break;
    case AllocationType::READ_BACK:
        createFlags = vma::AllocationCreateFlagBits::eHostAccessRandom |
                      vma::AllocationCreateFlagBits::eMapped;
    }
    vma::AllocationCreateInfo allocCreateInfo(createFlags,
                                              vma::MemoryUsage::eAuto);
    try
    {
        std::pair<vk::Image, vma::Allocation> imgData =
            m_current->createImage(imageCreateInfo, allocCreateInfo);
        image->image = std::make_unique<vk::Image>(imgData.first);
        image->allocation = std::make_unique<vma::Allocation>(imgData.second);

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

        // TODO: Add data copy to image

        return true;
    }
    catch (...)
    {
        handleVulkanException();
    }
    return false;
}

bool kirana::viewport::vulkan::Allocator::copyDataToBuffer(
    const AllocatedBuffer &buffer, const void *data, size_t dataOffset,
    size_t dataSize) const
{
    if (data == nullptr)
    {
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
                          "Cannot copy null data to buffer");
        return false;
    }
    const vk::MemoryPropertyFlags flags =
        m_current->getAllocationMemoryProperties(*buffer.allocation);
    try
    {
        if (flags & vk::MemoryPropertyFlagBits::eHostVisible)
        {
            const vma::AllocationInfo allocInfo =
                m_current->getAllocationInfo(*buffer.allocation);
            memcpy(reinterpret_cast<void *>(
                       reinterpret_cast<char *>(allocInfo.pMappedData) +
                       dataOffset),
                   data, dataSize);
        }
        else
        {
            // Create a staging buffer and copy data.
            vma::AllocationInfo stagingAllocInfo{};
            auto stagingBufferData = m_current->createBuffer(
                vk::BufferCreateInfo{{},
                                     dataSize,
                                     vk::BufferUsageFlagBits::eTransferSrc},
                vma::AllocationCreateInfo{
                    vma::AllocationCreateFlagBits::eHostAccessSequentialWrite |
                        vma::AllocationCreateFlagBits::eMapped,
                    vma::MemoryUsage::eAuto},
                &stagingAllocInfo);

            memcpy(stagingAllocInfo.pMappedData, data, dataSize);
            m_current->flushAllocation(stagingBufferData.second, 0,
                                       VK_WHOLE_SIZE);
            bool result = copyBuffer(stagingBufferData.first, *buffer.buffer,
                                     dataSize, 0, dataOffset);

            m_current->destroyBuffer(stagingBufferData.first,
                                     stagingBufferData.second);
        }
        return true;
    }
    catch (...)
    {
        handleVulkanException();
    }
    return false;
}

bool kirana::viewport::vulkan::Allocator::copyBuffer(
    const vk::Buffer &stagingBuffer, const vk::Buffer &destBuffer,
    vk::DeviceSize size, vk::DeviceSize srcOffset,
    vk::DeviceSize dstOffset) const
{
    // TODO: Use pipeline barriers instead of fence.
    VK_HANDLE_RESULT(
        m_device->current.waitForFences(
            m_commandFence, true, constants::VULKAN_COPY_BUFFER_WAIT_TIMEOUT),
        "Failed to wait for copy fence")
    m_device->current.resetFences(m_commandFence);
    m_commandPool->reset();
    m_commandBuffers->begin();
    vk::BufferCopy region(srcOffset, dstOffset, size);
    m_commandBuffers->copyBuffer(stagingBuffer, destBuffer, {region});
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
    if (buffer.buffer)
        m_current->destroyBuffer(*buffer.buffer, *buffer.allocation);
}

void kirana::viewport::vulkan::Allocator::free(const AllocateImage &image) const
{
    if (image.image)
        m_current->destroyImage(*image.image, *image.allocation);
}