#include "descriptor_set_layout.hpp"

#include "device.hpp"
#include "vulkan_utils.hpp"

kirana::viewport::vulkan::DescriptorSetLayout::DescriptorSetLayout(
    const Device *device)
    : m_isInitialized{false}, m_device{device}
{
    vk::DescriptorSetLayoutBinding camBuffer(
        0, vk::DescriptorType::eUniformBuffer, 1,
        vk::ShaderStageFlagBits::eVertex);

    vk::DescriptorSetLayoutCreateInfo createInfo({}, camBuffer);

    try
    {
        m_current = m_device->current.createDescriptorSetLayout(createInfo);
        m_isInitialized = true;
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                          "Descriptor Set Layout created");
    }
    catch(...)
    {
        handleVulkanException();
    }
}

kirana::viewport::vulkan::DescriptorSetLayout::~DescriptorSetLayout()
{
    if(m_device)
    {
        if(m_current)
        {
            m_device->current.destroyDescriptorSetLayout(m_current);
            Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                              "Descriptor Set Layout destroyed");
        }
    }
}