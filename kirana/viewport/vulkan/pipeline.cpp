#include "pipeline.hpp"

#include "device.hpp"
#include "renderpass.hpp"
#include "shader.hpp"
#include "pipeline_layout.hpp"

kirana::viewport::vulkan::Pipeline::Pipeline(
    const Device *const device, const RenderPass *const renderPass,
    const std::vector<Shader *> &shaders,
    const PipelineLayout *const pipelineLayout,
    const std::array<int, 2> &windowResolution)
    : m_isInitialized{false}, m_device{device}, m_renderPass{renderPass},
      m_shaders{shaders}, m_pipelineLayout{pipelineLayout}
{
    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
    for (const auto &s : shaders)
    {
        if (s->compute)
        {
            vk::PipelineShaderStageCreateInfo compute(
                {}, vk::ShaderStageFlagBits::eCompute, s->compute,
                constants::VULKAN_SHADER_MAIN_FUNC_NAME);
            shaderStages.push_back(compute);
        }
        if (s->vertex)
        {
            vk::PipelineShaderStageCreateInfo vertex(
                {}, vk::ShaderStageFlagBits::eVertex, s->vertex,
                constants::VULKAN_SHADER_MAIN_FUNC_NAME);
            shaderStages.push_back(vertex);
        }
        if (s->fragment)
        {
            vk::PipelineShaderStageCreateInfo fragment(
                {}, vk::ShaderStageFlagBits::eFragment, s->fragment,
                constants::VULKAN_SHADER_MAIN_FUNC_NAME);
            shaderStages.push_back(fragment);
        }
    }

    vk::PipelineVertexInputStateCreateInfo vertexInput({}, {}, {});

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly(
        {}, vk::PrimitiveTopology::eTriangleList, false);

    vk::Viewport vp(0.0f, 0.0f, static_cast<float>(windowResolution[0]),
                    static_cast<float>(windowResolution[1]), 0.0f, 1.0f);
    vk::Rect2D scissor({0, 0}, {static_cast<uint32_t>(windowResolution[0]),
                                static_cast<uint32_t>(windowResolution[1])});
    vk::PipelineViewportStateCreateInfo viewport({}, vp, scissor);

    vk::PipelineRasterizationStateCreateInfo rasterizer(
        {}, false, false, vk::PolygonMode::eFill, vk::CullModeFlagBits::eNone,
        vk::FrontFace::eClockwise, false, 0.0f, 0.0f, 0.0f, 1.0f);

    vk::PipelineMultisampleStateCreateInfo msaa(
        {}, vk::SampleCountFlagBits::e1, false, 1.0f, nullptr, false, false);

    vk::PipelineColorBlendAttachmentState attachment(false);
    attachment.setColorWriteMask(
        vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
        vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
    vk::PipelineColorBlendStateCreateInfo colorBlend(
        {}, false, vk::LogicOp::eCopy, attachment);

    vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo(
        vk::PipelineCreateFlags(), shaderStages, &vertexInput, &inputAssembly,
        nullptr, &viewport, &rasterizer, &msaa, nullptr, &colorBlend, nullptr,
        m_pipelineLayout->current, m_renderPass->current, 0);

    try
    {
        m_current =
            m_device->current
                .createGraphicsPipeline(nullptr, graphicsPipelineCreateInfo)
                .value;
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::debug,
                          "Pipeline created");
    }
    catch (...)
    {
        handleVulkanException();
    }
}

kirana::viewport::vulkan::Pipeline::~Pipeline()
{
    if (m_device)
    {
        if (m_current)
        {
            m_device->current.destroyPipeline(m_current);
            Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::debug,
                              "Pipeline destroyed");
        }
    }
}