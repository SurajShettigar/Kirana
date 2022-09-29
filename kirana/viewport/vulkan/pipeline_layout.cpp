#include "pipeline_layout.hpp"
#include "device.hpp"
#include "vulkan_utils.hpp"
#include "vulkan_types.hpp"

kirana::viewport::vulkan::PipelineLayout::PipelineLayout(
    const Device *const device)
    : m_isInitialized{false}, m_device{device}
{
    try
    {
        // TODO: Temporary solution for push constants.
        vk::PushConstantRange meshPushConstants(
            vk::ShaderStageFlagBits::eVertex, 0, sizeof(MeshPushConstants));

        m_current = m_device->current.createPipelineLayout(
            vk::PipelineLayoutCreateInfo({}, {}, meshPushConstants));
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::debug,
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

            Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::debug,
                              "Pipeline layout destroyed");
        }
    }
}