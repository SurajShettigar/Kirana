#include "raytrace_pipeline.hpp"
#include "device.hpp"
#include "renderpass.hpp"
#include "shader.hpp"
#include "pipeline_layout.hpp"
#include "vulkan_utils.hpp"

bool kirana::viewport::vulkan::RaytracePipeline::build()
{
    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
    for (const auto &m : m_shader->getAllModules())
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

    vk::RayTracingShaderGroupCreateInfoKHR shaderGroup {};
    shaderGroup.anyHitShader = VK_SHADER_UNUSED_KHR;
    shaderGroup.closestHitShader = VK_SHADER_UNUSED_KHR;
    shaderGroup.generalShader = VK_SHADER_UNUSED_KHR;
    shaderGroup.intersectionShader = VK_SHADER_UNUSED_KHR;

    // TODO: Create Raytrace pipeline
}

kirana::viewport::vulkan::RaytracePipeline::RaytracePipeline(
    const Device *device, const RenderPass *renderPass, const Shader *shader,
    const PipelineLayout *pipelineLayout)
    : Pipeline(device, renderPass, shader, pipelineLayout, {}, {})
{
}
kirana::viewport::vulkan::RaytracePipeline::~RaytracePipeline()
{
}