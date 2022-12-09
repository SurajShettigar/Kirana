#include "pipeline.hpp"
#include "device.hpp"
#include "renderpass.hpp"
#include "shader.hpp"
#include "pipeline_layout.hpp"
#include "vulkan_utils.hpp"
#include "vulkan_types.hpp"

kirana::viewport::vulkan::Pipeline::Pipeline(
    const Device *const device, const RenderPass *const renderPass,
    const Shader *const shader, const PipelineLayout *const pipelineLayout,
    const VertexInputDescription &vertexInputDesc,
    const PipelineProperties &pipelineProperties)
    : m_isInitialized{false}, m_device{device}, m_renderPass{renderPass},
      m_shader{shader}, m_pipelineLayout{pipelineLayout}
{
    build(vertexInputDesc.bindings, vertexInputDesc.attributes,
          pipelineProperties);
}

kirana::viewport::vulkan::Pipeline::~Pipeline()
{
    if (m_device)
    {
        if (m_current)
        {
            m_device->current.destroyPipeline(m_current);
            Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                              "Pipeline destroyed");
        }
    }
}

bool kirana::viewport::vulkan::Pipeline::build(
    const std::vector<vk::VertexInputBindingDescription> &vertexBindings,
    const std::vector<vk::VertexInputAttributeDescription> &vertexAttributes,
    const PipelineProperties &properties)
{
    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
    if (m_shader->compute)
    {
        vk::PipelineShaderStageCreateInfo compute(
            {}, vk::ShaderStageFlagBits::eCompute, m_shader->compute,
            constants::VULKAN_SHADER_MAIN_FUNC_NAME);
        shaderStages.push_back(compute);
    }
    if (m_shader->vertex)
    {
        vk::PipelineShaderStageCreateInfo vertex(
            {}, vk::ShaderStageFlagBits::eVertex, m_shader->vertex,
            constants::VULKAN_SHADER_MAIN_FUNC_NAME);
        shaderStages.push_back(vertex);
    }
    if (m_shader->fragment)
    {
        vk::PipelineShaderStageCreateInfo fragment(
            {}, vk::ShaderStageFlagBits::eFragment, m_shader->fragment,
            constants::VULKAN_SHADER_MAIN_FUNC_NAME);
        shaderStages.push_back(fragment);
    }

    vk::PipelineVertexInputStateCreateInfo vertexInput({}, vertexBindings,
                                                       vertexAttributes);

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly(
        {}, properties.primitiveType, false);

    const std::array<uint32_t, 2> &res = m_renderPass->getSurfaceResolution();
    vk::Viewport vp(0.0f, 0.0f, static_cast<float>(res[0]),
                    static_cast<float>(res[1]), 0.0f, 1.0f);
    vk::Rect2D scissor({0, 0}, {res[0], res[1]});
    vk::PipelineViewportStateCreateInfo viewport({}, vp, scissor);

    vk::PipelineRasterizationStateCreateInfo rasterizer(
        {}, false, false, properties.polygonMode, properties.cullMode,
        vk::FrontFace::eCounterClockwise, false, 0.0f, 0.0f, 0.0f,
        properties.lineWidth);

    vk::PipelineMultisampleStateCreateInfo msaa({}, properties.msaaLevel, false,
                                                1.0f, nullptr, false, false);

    vk::StencilOpState stencilState{properties.stencilFailOp,
                                    properties.stencilPassOp,
                                    properties.stencilDepthFailOp,
                                    properties.stencilCompareOp,
                                    0xFF,
                                    0xFF,
                                    properties.stencilReference};

    vk::PipelineDepthStencilStateCreateInfo depthStencil(
        {}, properties.enableDepth, properties.writeDepth,
        properties.depthCompareOp, false, properties.stencilTest, stencilState,
        stencilState, 0.0f, 1.0f);

    vk::PipelineColorBlendAttachmentState attachment{
        properties.alphaBlending,
        vk::BlendFactor::eSrcAlpha,
        vk::BlendFactor::eOneMinusSrcAlpha,
        vk::BlendOp::eAdd,
        vk::BlendFactor::eOne,
        vk::BlendFactor::eZero,
        vk::BlendOp::eAdd};

    attachment.setColorWriteMask(
        vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
        vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
    vk::PipelineColorBlendStateCreateInfo colorBlend(
        {}, false, vk::LogicOp::eCopy, attachment);

    vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo(
        vk::PipelineCreateFlags(), shaderStages, &vertexInput, &inputAssembly,
        nullptr, &viewport, &rasterizer, &msaa, &depthStencil, &colorBlend,
        nullptr, m_pipelineLayout->current, m_renderPass->current, 0);

    try
    {
        m_current =
            m_device->current
                .createGraphicsPipeline(nullptr, graphicsPipelineCreateInfo)
                .value;
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                          "Pipeline created");
        return true;
    }
    catch (...)
    {
        handleVulkanException();
    }
    return false;
}

/*
bool kirana::viewport::vulkan::Pipeline::rebuild(
    const VertexInputDescription &vertexDesc)
{
    if (m_current)
        m_device->current.destroyPipeline(m_current);
    return build(vertexDesc.bindings, vertexDesc.attributes);
}*/
