#include "pipeline_layout.hpp"
#include "device.hpp"
#include "descriptor_set_layout.hpp"
#include "vulkan_utils.hpp"
#include "push_constant.hpp"

kirana::viewport::vulkan::PipelineLayout::PipelineLayout(
    const Device *const device,
    const std::vector<const DescriptorSetLayout *> &descriptorSetLayouts,
    const std::vector<const PushConstantBase *> &pushConstants)
    : m_isInitialized{false}, m_device{device}
{
    try
    {
        std::vector<vk::PushConstantRange> pc;
        for (auto &p : pushConstants)
            pc.push_back(p->getRange());

        std::vector<vk::DescriptorSetLayout> layouts;
        for (const auto &l : descriptorSetLayouts)
            layouts.emplace_back(l->current);
        m_current = m_device->current.createPipelineLayout(
            vk::PipelineLayoutCreateInfo({}, layouts, pc));
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                          "Pipeline layout created");
        m_isInitialized = true;
    }
    catch (...)
    {
        handleVulkanException();
    }
}
kirana::viewport::vulkan::PipelineLayout::~PipelineLayout()
{
    if (m_device)
    {
        if (m_current)
        {
            m_device->current.destroyPipelineLayout(m_current);

            Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                              "Pipeline layout destroyed");
        }
    }
}