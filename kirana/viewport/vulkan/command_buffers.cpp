#include "command_buffers.hpp"

#include "device.hpp"
#include "command_pool.hpp"

kirana::viewport::vulkan::CommandBuffers::CommandBuffers(
    const Device *const device, const CommandPool *const commandPool,
    size_t count, vk::CommandBufferLevel level)
    : m_isInitialized{false}, m_device{device}, m_commandPool{commandPool}
{
    vk::CommandBufferAllocateInfo allocateInfo(m_commandPool->current, level,
                                               static_cast<uint32_t>(count));
    try
    {
        m_current = m_device->current.allocateCommandBuffers(allocateInfo);
        m_isInitialized = true;
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::debug,
                          "Command Buffers allocated");
    }
    catch (...)
    {
        handleVulkanException();
    }
}