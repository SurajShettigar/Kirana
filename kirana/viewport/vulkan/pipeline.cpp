#include "pipeline.hpp"
#include "device.hpp"
#include "renderpass.hpp"
#include "shader.hpp"
#include "pipeline_layout.hpp"
#include "vulkan_utils.hpp"

kirana::viewport::vulkan::Pipeline::Pipeline(
    const Device *const device, const RenderPass *const renderPass,
    const std::vector<const DescriptorSetLayout *> &descriptorSetLayouts,
    std::string name, std::string shaderName,
    VertexInputDescription vertexInputDesc, Properties properties)
    : m_isInitialized{false}, m_device{device}, m_renderPass{renderPass},
      m_pipelineLayout{new PipelineLayout(m_device, descriptorSetLayouts)},
      m_name{std::move(name)}, m_shaderName{std::move(shaderName)},
      m_vertexInputDesc{std::move(vertexInputDesc)}, m_properties{properties}
{
    if (m_pipelineLayout->isInitialized)
        m_isInitialized = build();
}

kirana::viewport::vulkan::Pipeline::~Pipeline()
{
    if (m_device)
    {
        if (m_pipelineLayout)
        {
            delete m_pipelineLayout;
            m_pipelineLayout = nullptr;
        }
        if (m_current)
        {
            m_device->current.destroyPipeline(m_current);
            Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                              "Pipeline destroyed");
        }
    }
}

bool kirana::viewport::vulkan::Pipeline::build()
{
    const Shader shader(m_device, m_shaderName);

    if (!shader.isInitialized)
        return false;

    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
    for (const auto &m : shader.getAllModules())
    {
        vk::ShaderStageFlagBits stageFlag = vk::ShaderStageFlagBits::eVertex;
        switch (m.first)
        {
        case ShaderStage::COMPUTE:
            stageFlag = vk::ShaderStageFlagBits::eCompute;
            break;
        case ShaderStage::VERTEX:
            stageFlag = vk::ShaderStageFlagBits::eVertex;
            break;
        case ShaderStage::FRAGMENT:
            stageFlag = vk::ShaderStageFlagBits::eFragment;
            break;
        case ShaderStage::RAYTRACE_RAY_GEN:
            stageFlag = vk::ShaderStageFlagBits::eRaygenKHR;
            break;
        case ShaderStage::RAYTRACE_MISS:
            stageFlag = vk::ShaderStageFlagBits::eMissKHR;
            break;
        case ShaderStage::RAYTRACE_CLOSEST_HIT:
            stageFlag = vk::ShaderStageFlagBits::eClosestHitKHR;
            break;
        default:
            break;
        }
        shaderStages.emplace_back(vk::PipelineShaderStageCreateInfo(
            {}, stageFlag, m.second, constants::VULKAN_SHADER_MAIN_FUNC_NAME));
    }

    const vk::PipelineVertexInputStateCreateInfo vertexInput(
        {}, m_vertexInputDesc.bindings, m_vertexInputDesc.attributes);

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

    const vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo(
        vk::PipelineCreateFlags(), shaderStages, &vertexInput, &inputAssembly,
        nullptr, &viewport, &rasterizer, &msaa, &depthStencil, &colorBlend,
        nullptr, m_pipelineLayout->current, m_renderPass->current, 0);

    auto result = m_device->current.createGraphicsPipeline(
        nullptr, graphicsPipelineCreateInfo);
    if (result.result != vk::Result::eSuccess)
    {
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
                          "Failed to create pipeline for material: " + m_name);
        return false;
    }

    m_current = result.value;
    Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                      "Pipeline created for material: " + m_name);
    return true;
}
