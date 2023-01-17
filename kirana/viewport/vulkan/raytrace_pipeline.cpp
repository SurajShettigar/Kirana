#include "raytrace_pipeline.hpp"
#include "device.hpp"
#include "renderpass.hpp"
#include "shader.hpp"
#include "pipeline_layout.hpp"
#include "vulkan_utils.hpp"
#include "push_constant.hpp"

bool kirana::viewport::vulkan::RaytracePipeline::build()
{
    if (m_device->raytracingProperties.maxRayRecursionDepth <
        m_maxRecursionDepth)
    {
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
                          "Failed to create Raytrace Pipeline for material: " +
                              m_name + ". Max Recursion Depth Exceeded");
        return false;
    }

    const Shader shader(m_device, m_shaderName);

    if (!shader.isInitialized)
        return false;


    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
    std::vector<vk::RayTracingShaderGroupCreateInfoKHR> shaderGroups;

    vk::RayTracingShaderGroupCreateInfoKHR shaderGroup{};
    shaderGroup.anyHitShader = VK_SHADER_UNUSED_KHR;
    shaderGroup.closestHitShader = VK_SHADER_UNUSED_KHR;
    shaderGroup.generalShader = VK_SHADER_UNUSED_KHR;
    shaderGroup.intersectionShader = VK_SHADER_UNUSED_KHR;
    for (const auto &m : shader.getAllModules())
    {
        std::string stageName = "";
        vk::ShaderStageFlagBits stageFlag = vk::ShaderStageFlagBits::eVertex;
        switch (m.first)
        {
        case ShaderStage::RAYTRACE_RAY_GEN:
            stageFlag = vk::ShaderStageFlagBits::eRaygenKHR;
            stageName = "RayGen";
            break;
        case ShaderStage::RAYTRACE_MISS:
            stageFlag = vk::ShaderStageFlagBits::eMissKHR;
            stageName = "Miss";
            break;
        case ShaderStage::RAYTRACE_MISS_SHADOW:
            stageFlag = vk::ShaderStageFlagBits::eMissKHR;
            stageName = "MissShadow";
            break;
        case ShaderStage::RAYTRACE_CLOSEST_HIT:
            stageFlag = vk::ShaderStageFlagBits::eClosestHitKHR;
            stageName = "ClosestHit";
            break;
        default:
            break;
        }
        m_device->setDebugObjectName(m.second, "ShaderModule_" + m_shaderName +
                                                   "_" + stageName);
        shaderStages.emplace_back(vk::PipelineShaderStageCreateInfo(
            {}, stageFlag, m.second, constants::VULKAN_SHADER_MAIN_FUNC_NAME));
    }

    // Ray-Gen Shader
    shaderGroup.type = vk::RayTracingShaderGroupTypeKHR::eGeneral;
    shaderGroup.generalShader = static_cast<uint32_t>(
        shaderGroups.size()); // Index/Order of shader stage
    shaderGroups.push_back(shaderGroup);

    // Miss Shader
    shaderGroup.type = vk::RayTracingShaderGroupTypeKHR::eGeneral;
    shaderGroup.generalShader = static_cast<uint32_t>(shaderGroups.size());
    shaderGroups.push_back(shaderGroup);

    // Miss Shadow Shader
//    shaderGroup.type = vk::RayTracingShaderGroupTypeKHR::eGeneral;
//    shaderGroup.generalShader = static_cast<uint32_t>(shaderGroups.size());
//    shaderGroups.push_back(shaderGroup);

    // Hit group
    shaderGroup.type = vk::RayTracingShaderGroupTypeKHR::eTrianglesHitGroup;
    shaderGroup.closestHitShader = static_cast<uint32_t>(shaderGroups.size());
    shaderGroup.generalShader = VK_SHADER_UNUSED_KHR;
    shaderGroups.push_back(shaderGroup);

    vk::RayTracingPipelineCreateInfoKHR createInfo(
        {}, shaderStages, shaderGroups, m_maxRecursionDepth);
    createInfo.layout = m_pipelineLayout->current;

    auto result =
        m_device->current.createRayTracingPipelineKHR({}, {}, createInfo);
    if (result.result != vk::Result::eSuccess)
    {
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
                          "Failed to create Raytrace Pipeline for material: " +
                              m_name);
        return false;
    }
    m_current = result.value;
    Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                      "Raytrace Pipeline created for material: " + m_name);
    return true;
}

kirana::viewport::vulkan::RaytracePipeline::RaytracePipeline(
    const Device *const device, const RenderPass *const renderPass,
    const std::vector<const DescriptorSetLayout *> &descriptorSetLayouts,
    const std::vector<const PushConstantBase *> &pushConstants,
    std::string name, std::string shaderName)
    : Pipeline(device, renderPass, descriptorSetLayouts, pushConstants,
               std::move(name), std::move(shaderName))
{
    if (m_pipelineLayout->isInitialized)
        m_isInitialized = build();
    if (m_isInitialized)
        m_device->setDebugObjectName(m_current, "RaytracePipeline_" + m_name);
}

kirana::viewport::vulkan::RaytracePipeline::~RaytracePipeline()
{
    Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                      "Raytrace Pipeline destroyed for material: " + m_name);
}