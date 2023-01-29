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
        m_properties.maxRecursionDepth)
    {
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
                          "Failed to create Raytrace Pipeline for shader: " +
                              m_name + ". Max Recursion Depth Exceeded");
        return false;
    }

    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
    std::vector<vk::RayTracingShaderGroupCreateInfoKHR> shaderGroups;
    uint32_t stageIndex = 0;
    for (const auto &stage : m_shader->getAllModules())
    {
        for (const auto &m : stage.second)
        {
            shaderStages.emplace_back(vk::PipelineShaderStageCreateInfo(
                {}, stage.first, m, constants::VULKAN_SHADER_MAIN_FUNC_NAME));

            switch (stage.first)
            {
            case vk::ShaderStageFlagBits::eRaygenKHR:
            case vk::ShaderStageFlagBits::eMissKHR:
            case vk::ShaderStageFlagBits::eCallableKHR: {
                shaderGroups.emplace_back(
                    vk::RayTracingShaderGroupTypeKHR::eGeneral, stageIndex++,
                    VK_SHADER_UNUSED_KHR, VK_SHADER_UNUSED_KHR,
                    VK_SHADER_UNUSED_KHR);
            }
            break;
            case vk::ShaderStageFlagBits::eClosestHitKHR: {
                shaderGroups.emplace_back(
                    vk::RayTracingShaderGroupTypeKHR::eTrianglesHitGroup,
                    VK_SHADER_UNUSED_KHR, stageIndex++, VK_SHADER_UNUSED_KHR,
                    VK_SHADER_UNUSED_KHR);
            }
            break;
            case vk::ShaderStageFlagBits::eAnyHitKHR: {
                shaderGroups.emplace_back(
                    vk::RayTracingShaderGroupTypeKHR::eTrianglesHitGroup,
                    VK_SHADER_UNUSED_KHR, VK_SHADER_UNUSED_KHR, stageIndex++,
                    VK_SHADER_UNUSED_KHR);
            }
            break;
            case vk::ShaderStageFlagBits::eIntersectionKHR: {
                shaderGroups.emplace_back(
                    vk::RayTracingShaderGroupTypeKHR::eProceduralHitGroup,
                    VK_SHADER_UNUSED_KHR, VK_SHADER_UNUSED_KHR,
                    VK_SHADER_UNUSED_KHR, stageIndex++);
            }
            break;
            default:
                break;
            }
        }
    }

    m_createInfo =
        vk::RayTracingPipelineCreateInfoKHR{{},
                                            shaderStages,
                                            shaderGroups,
                                            m_properties.maxRecursionDepth};
    m_createInfo.layout = m_shader->getPipelineLayout().current;

    auto result =
        m_device->current.createRayTracingPipelineKHR({}, {}, m_createInfo);
    if (result.result != vk::Result::eSuccess)
    {
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
                          "Failed to create Raytrace Pipeline for shader: " +
                              m_name);
        return false;
    }
    m_current = result.value;
    Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                      "Raytrace Pipeline created for shader: " + m_name);
    return true;
}

kirana::viewport::vulkan::RaytracePipeline::RaytracePipeline(
    const Device *const device, const RenderPass *const renderPass,
    const Shader *const shader, Properties properties)
    : Pipeline(device, renderPass, shader, vulkan::ShadingPipeline::RAYTRACE),
      m_properties{std::move(properties)}
{
    m_isInitialized = build();
    if (m_isInitialized)
        m_device->setDebugObjectName(m_current, "RaytracePipeline_" + m_name);
}

kirana::viewport::vulkan::RaytracePipeline::~RaytracePipeline()
{
    Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::trace,
                      "Raytrace Pipeline destroyed for shader: " + m_name);
}