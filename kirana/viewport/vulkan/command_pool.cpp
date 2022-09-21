#include "command_pool.hpp"

#include "device.hpp"

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
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::debug,
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
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::debug,
                          "Command Pool destroyed");
    }
}