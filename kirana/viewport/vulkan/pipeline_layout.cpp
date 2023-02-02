#include "pipeline_layout.hpp"
#include "device.hpp"
#include "descriptor_set_layout.hpp"
#include "vulkan_utils.hpp"
#include "push_constant.hpp"


kirana::viewport::vulkan::PipelineLayout::PipelineLayout(
    const Device *const device,
    std::vector<const DescriptorSetLayout *> descriptorSetLayouts,
    std::vector<const PushConstantBase *> pushConstants)
    : m_isInitialized{false}, m_device{device},
      m_descLayouts{std::move(descriptorSetLayouts)}, m_pushConstants{std::move(
                                                          pushConstants)}
{
    try
    {
        std::vector<vk::PushConstantRange> pc;
        for (auto &p : m_pushConstants)
            pc.push_back(p->getRange());

        std::vector<vk::DescriptorSetLayout> layouts;
        for (const auto &l : m_descLayouts)
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
            for (auto &p : m_pushConstants)
            {
                if (p)
                {
                    delete p;
                    p = nullptr;
                }
            }
            for (auto &d : m_descLayouts)
            {
                if (d)
                {
                    delete d;
                    d = nullptr;
                }
            }
            m_device->current.destroyPipelineLayout(m_current);

            Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                              "Pipeline layout destroyed");
        }
    }
}

bool kirana::viewport::vulkan::PipelineLayout::getDefaultPipelineLayout(
    const Device *const device, vulkan::ShadingPipeline shadingPipeline,
    const PipelineLayout *&layout)
{
    std::vector<const DescriptorSetLayout *> descLayouts(2);
    DescriptorSetLayout::getDefaultDescriptorLayout(
        device, DescriptorLayoutType::GLOBAL, shadingPipeline, descLayouts[0]);
    //    DescriptorSetLayout::getDefaultDescriptorLayout(
    //        device, DescriptorLayoutType::MATERIAL, shadingPipeline,
    //        descLayouts[1]);
    DescriptorSetLayout::getDefaultDescriptorLayout(
        device, DescriptorLayoutType::OBJECT, shadingPipeline, descLayouts[1]);

    std::vector<const PushConstantBase *> pushConstants(1);
    switch (shadingPipeline)
    {
    case ShadingPipeline::RAYTRACE: {
        pushConstants[0] = new PushConstant<vulkan::PushConstantRaytrace>(
            {}, vulkan::PUSH_CONSTANT_RAYTRACE_SHADER_STAGES);
    }
    break;
    case ShadingPipeline::RASTER:
    default: {
        pushConstants[0] = new PushConstant<vulkan::PushConstantRaster>(
            {}, vulkan::PUSH_CONSTANT_RASTER_SHADER_STAGES);
    }
    break;
    }

    layout = new PipelineLayout(device, std::move(descLayouts),
                                std::move(pushConstants));
    return layout->m_isInitialized;
}