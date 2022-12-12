#include "descriptor_set_layout.hpp"

#include "device.hpp"
#include "vulkan_utils.hpp"

kirana::viewport::vulkan::DescriptorSetLayout::DescriptorSetLayout(
    const Device *device, LayoutType layoutType)
    : m_isInitialized{false}, m_device{device}
{
    std::vector<vk::DescriptorSetLayoutBinding> bindings;
    switch (layoutType)
    {
    case LayoutType::GLOBAL: {
        const vk::DescriptorSetLayoutBinding camBuffer(
            0, vk::DescriptorType::eUniformBufferDynamic, 1,
            vk::ShaderStageFlagBits::eVertex);

        const vk::DescriptorSetLayoutBinding worldDataBuffer(
            1, vk::DescriptorType::eUniformBufferDynamic, 1,
            vk::ShaderStageFlagBits::eVertex |
                vk::ShaderStageFlagBits::eFragment);

        bindings.clear();
        bindings.emplace_back(camBuffer);
        bindings.emplace_back(worldDataBuffer);
    }
    break;
    case LayoutType::OBJECT: {
        const vk::DescriptorSetLayoutBinding objectBuffer(
            0, vk::DescriptorType::eStorageBufferDynamic, 1,
            vk::ShaderStageFlagBits::eVertex);
        bindings.clear();
        bindings.emplace_back(objectBuffer);
    }
    break;
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