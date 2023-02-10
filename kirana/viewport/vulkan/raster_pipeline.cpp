#include "raster_pipeline.hpp"
#include "device.hpp"
#include "renderpass.hpp"
#include "shader.hpp"
#include "pipeline_layout.hpp"
#include "vulkan_utils.hpp"
#include "push_constant.hpp"

bool kirana::viewport::vulkan::RasterPipeline::build()
{
    if (!m_shader->isInitialized)
        return false;

    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
    for (const auto &stage : m_shader->getAllModules())
        for (const auto &m : stage.second)
            shaderStages.emplace_back(vk::PipelineShaderStageCreateInfo(
                {}, stage.first, m, constants::VULKAN_SHADER_MAIN_FUNC_NAME));

    const vk::PipelineVertexInputStateCreateInfo vertexInput(
        {}, m_properties.vertexBindings, m_properties.vertexAttributes);

    const vk::PipelineInputAssemblyStateCreateInfo inputAssembly(
        {}, m_properties.primitiveType, false);

    const std::array<uint32_t, 2> &res = m_renderPass->getSurfaceResolution();
    const vk::Viewport vp(0.0f, 0.0f, static_cast<float>(res[0]),
                          static_cast<float>(res[1]), 0.0f, 1.0f);
    const vk::Rect2D scissor({0, 0}, {res[0], res[1]});
    const vk::PipelineViewportStateCreateInfo viewport({}, vp, scissor);

    const vk::PipelineRasterizationStateCreateInfo rasterizer(
        {}, false, false, m_properties.polygonMode, m_properties.cullMode,
        vk::FrontFace::eCounterClockwise, false, 0.0f, 0.0f, 0.0f,
        m_properties.lineWidth);

    const vk::PipelineMultisampleStateCreateInfo msaa(
        {}, m_properties.msaaLevel, false, 1.0f, nullptr, false, false);

    const vk::StencilOpState stencilState{m_properties.stencilFailOp,
                                          m_properties.stencilPassOp,
                                          m_properties.stencilDepthFailOp,
                                          m_properties.stencilCompareOp,
                                          0xFF,
                                          0xFF,
                                          m_properties.stencilReference};

    const vk::PipelineDepthStencilStateCreateInfo depthStencil(
        {}, m_properties.enableDepth, m_properties.writeDepth,
        m_properties.depthCompareOp, false, m_properties.stencilTest,
        stencilState, stencilState, 0.0f, 1.0f);

    vk::PipelineColorBlendAttachmentState attachment{
        m_properties.alphaBlending,
        vk::BlendFactor::eSrcAlpha,
        vk::BlendFactor::eOneMinusSrcAlpha,
        vk::BlendOp::eAdd,
        vk::BlendFactor::eOne,
        vk::BlendFactor::eZero,
        vk::BlendOp::eAdd};

    attachment.setColorWriteMask(
        vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
        vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);

    const vk::PipelineColorBlendStateCreateInfo colorBlend(
        {}, false, vk::LogicOp::eCopy, attachment);

    const std::vector<vk::DynamicState> dynamicStates{
        vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    const vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo(
        {}, dynamicStates);

    const vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo(
        vk::PipelineCreateFlags(), shaderStages, &vertexInput, &inputAssembly,
        nullptr, &viewport, &rasterizer, &msaa, &depthStencil, &colorBlend,
        &dynamicStateCreateInfo, m_shader->getPipelineLayout().current,
        m_renderPass->current, 0);

    auto result = m_device->current.createGraphicsPipeline(
        nullptr, graphicsPipelineCreateInfo);
    if (result.result != vk::Result::eSuccess)
    {
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
                          "Failed to create pipeline for shader: " + m_name);
        return false;
    }

    m_current = result.value;
    Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                      "Raster Pipeline created for shader: " + m_name);
    return true;
}

kirana::viewport::vulkan::RasterPipeline::RasterPipeline(
    const Device *device, const RenderPass *renderPass,
    const Shader *const shader, Properties properties)
    : Pipeline(device, renderPass, shader,
               vulkan::ShadingPipeline::RASTER),
      m_properties{std::move(properties)}
{
    m_isInitialized = build();
    if (m_isInitialized)
        m_device->setDebugObjectName(m_current, "Pipeline_" + m_name);
}

kirana::viewport::vulkan::RasterPipeline::~RasterPipeline()
{
    Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                      "Raster Pipeline destroyed for shader: " + m_name);
}