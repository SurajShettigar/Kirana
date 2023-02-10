#include "command_pool.hpp"
#include "device.hpp"
#include "command_buffers.hpp"

#include "vulkan_utils.hpp"

kirana::viewport::vulkan::CommandPool::CommandPool(
    const Device *const device, uint32_t queueFamilyIndex,
    vk::CommandPoolCreateFlagBits flags)
    : m_isInitialized{false}, m_device{device}
{
    vk::CommandPoolCreateInfo createInfo(vk::CommandPoolCreateFlags(flags),
                                         queueFamilyIndex);

    try
    {
        m_current = m_device->current.createCommandPool(createInfo);
        m_isInitialized = true;
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                          "Command Pool created");
    }
    catch (...)
    {
        handleVulkanException();
    }
}

kirana::viewport::vulkan::CommandPool::~CommandPool()
{
    if (m_device && m_current)
    {
        m_device->current.destroyCommandPool(m_current);
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                          "Command Pool destroyed");
    }
}

bool kirana::viewport::vulkan::CommandPool::allocateCommandBuffers(
    const CommandBuffers *&commandBuffers, size_t count,
    vk::CommandBufferLevel level) const
{
    vk::CommandBufferAllocateInfo allocateInfo(m_current, level,
                                               static_cast<uint32_t>(count));
    try
    {
        commandBuffers = new CommandBuffers(
            m_device->current.allocateCommandBuffers(allocateInfo));
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                          "Command Buffers allocated");
    }
    catch (...)
    {
        handleVulkanException();
        return false;
    }
    return true;
}

void kirana::viewport::vulkan::CommandPool::reset(
    vk::CommandPoolResetFlags resetFlags) const
{
    m_device->current.resetCommandPool(m_current, resetFlags);
}

void kirana::viewport::vulkan::CommandPool::freeCommandBuffers(
    const CommandBuffers *&buffers) const
{
    m_device->current.freeCommandBuffers(m_current, buffers->current);
}