#include "pipeline_layout.hpp"
#include "device.hpp"
#include "descriptor_set_layout.hpp"
#include "vulkan_utils.hpp"
#include "vulkan_types.hpp"

kirana::viewport::vulkan::PipelineLayout::PipelineLayout(
    const Device *const device, const DescriptorSetLayout *globalDescSetLayout)
    : m_isInitialized{false}, m_device{device}, m_globalDescSetLayout{
                                                    globalDescSetLayout}
{
    try
    {
        // TODO: Temporary solution for push constants.
        vk::PushConstantRange meshPushConstants(
            vk::ShaderStageFlagBits::eVertex, 0, sizeof(MeshPushConstants));

        m_current =
            m_device->current.createPipelineLayout(vk::PipelineLayoutCreateInfo(
                {}, m_globalDescSetLayout->current, meshPushConstants));
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