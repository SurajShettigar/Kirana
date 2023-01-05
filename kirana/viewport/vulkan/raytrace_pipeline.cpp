#include "raytrace_pipeline.hpp"
#include "device.hpp"
#include "renderpass.hpp"
#include "shader.hpp"
#include "pipeline_layout.hpp"
#include "vulkan_utils.hpp"

bool kirana::viewport::vulkan::RaytracePipeline::build()
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

    std::vector<vk::RayTracingShaderGroupCreateInfoKHR> shaderGroups;

    vk::RayTracingShaderGroupCreateInfoKHR shaderGroup{};
    shaderGroup.anyHitShader = VK_SHADER_UNUSED_KHR;
    shaderGroup.closestHitShader = VK_SHADER_UNUSED_KHR;
    shaderGroup.generalShader = VK_SHADER_UNUSED_KHR;
    shaderGroup.intersectionShader = VK_SHADER_UNUSED_KHR;

    // Ray-Gen Shader
    shaderGroup.type = vk::RayTracingShaderGroupTypeKHR::eGeneral;
    shaderGroup.generalShader = 0; // Index/Order of shader stage
    shaderGroups.push_back(shaderGroup);

    // Miss Shader
    shaderGroup.type = vk::RayTracingShaderGroupTypeKHR::eGeneral;
    shaderGroup.generalShader = 1;
    shaderGroups.push_back(shaderGroup);

    // Hit group
    shaderGroup.type = vk::RayTracingShaderGroupTypeKHR::eTrianglesHitGroup;
    shaderGroup.closestHitShader = 2;
    shaderGroup.generalShader = VK_SHADER_UNUSED_KHR;
    shaderGroups.push_back(shaderGroup);

    vk::RayTracingPipelineCreateInfoKHR createInfo({}, shaderStages,
                                                   shaderGroups, 1);
    createInfo.layout = m_pipelineLayout->current;

    auto result =
        m_device->current.createRayTracingPipelineKHR({}, {}, createInfo);
    if (result.result != vk::Result::eSuccess)
    {
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
                          "Failed to create Raytrace Pipeline");
        return false;
    }
    m_current = result.value;
    Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                      "Raytrace Pipeline created");
    return true;
}

kirana::viewport::vulkan::RaytracePipeline::RaytracePipeline(
    const Device *const device, const RenderPass *const renderPass,
    const std::vector<const DescriptorSetLayout *> &descriptorSetLayouts,
    std::string name, std::string shaderName)
    : Pipeline(device, renderPass, descriptorSetLayouts, std::move(name),
               std::move(shaderName), {}, {})
{
}

kirana::viewport::vulkan::RaytracePipeline::~RaytracePipeline()
{
}