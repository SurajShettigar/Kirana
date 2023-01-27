#include "descriptor_set_layout.hpp"

#include "device.hpp"
#include "vulkan_utils.hpp"

kirana::viewport::vulkan::DescriptorSetLayout::DescriptorSetLayout(
    const Device *device, const std::vector<DescriptorData> &bindingData)
    : m_isInitialized{false}, m_device{device}
{
    std::vector<vk::DescriptorSetLayoutBinding> bindings(bindingData.size());
    for (size_t i = 0; i < bindingData.size(); i++)
    {
        bindings.emplace_back(vk::DescriptorSetLayoutBinding{
            bindingData[i].binding, bindingData[i].type, 1,
            bindingData[i].stages});
    }

    const vk::DescriptorSetLayoutCreateInfo createInfo({}, bindings);
    try
    {
        m_current = m_device->current.createDescriptorSetLayout(createInfo);
        m_isInitialized = true;
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                          "Descriptor Set Layout created");
    }
    catch (...)
    {
        handleVulkanException();
    }
}

kirana::viewport::vulkan::DescriptorSetLayout::~DescriptorSetLayout()
{
    if (m_device)
    {
        if (m_current)
        {
            m_device->current.destroyDescriptorSetLayout(m_current);
            Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                              "Descriptor Set Layout destroyed");
        }
    }
}