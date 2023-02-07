#include "raytrace_pipeline.hpp"
#include "device.hpp"
#include "renderpass.hpp"
#include "shader.hpp"
#include "pipeline_layout.hpp"
#include "vulkan_utils.hpp"
#include "push_constant.hpp"

bool kirana::viewport::vulkan::RaytracePipeline::build()
{
    if (!m_shader->isInitialized)
        return false;

    if (m_device->raytracingProperties.maxRayRecursionDepth <
        m_properties.maxRecursionDepth)
    {
        Logger::get().log(constants::LOG_CHANNEL_VULKAN, LogSeverity::error,
                          "Failed to create Raytrace Pipeline for shader: " +
                              m_name + ". Max Recursion Depth Exceeded");
        return false;
    }

    m_shaderStages.clear();
    m_shaderGroups.clear();
    uint32_t stageIndex = 0;
    for (const auto &stage : m_shader->getAllModules())
    {
        for (const auto &m : stage.second)
        {
            m_shaderStages.emplace_back(vk::PipelineShaderStageCreateInfo(
                {}, stage.first, m, constants::VULKAN_SHADER_MAIN_FUNC_NAME));

            switch (stage.first)
            {
            case vk::ShaderStageFlagBits::eRaygenKHR:
            case vk::ShaderStageFlagBits::eMissKHR:
            case vk::ShaderStageFlagBits::eCallableKHR: {
                m_shaderGroups.emplace_back(
                    vk::RayTracingShaderGroupTypeKHR::eGeneral, stageIndex++,
                    VK_SHADER_UNUSED_KHR, VK_SHADER_UNUSED_KHR,
                    VK_SHADER_UNUSED_KHR);
            }
            break;
            case vk::ShaderStageFlagBits::eClosestHitKHR: {
                m_shaderGroups.emplace_back(
                    vk::RayTracingShaderGroupTypeKHR::eTrianglesHitGroup,
                    VK_SHADER_UNUSED_KHR, stageIndex++, VK_SHADER_UNUSED_KHR,
                    VK_SHADER_UNUSED_KHR);
            }
            break;
            case vk::ShaderStageFlagBits::eAnyHitKHR: {
                m_shaderGroups.emplace_back(
                    vk::RayTracingShaderGroupTypeKHR::eTrianglesHitGroup,
                    VK_SHADER_UNUSED_KHR, VK_SHADER_UNUSED_KHR, stageIndex++,
                    VK_SHADER_UNUSED_KHR);
            }
            break;
            case vk::ShaderStageFlagBits::eIntersectionKHR: {
                m_shaderGroups.emplace_back(
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

    vk::RayTracingPipelineCreateInfoKHR createInfo =
        vk::RayTracingPipelineCreateInfoKHR{{},
                                            m_shaderStages,
                                            m_shaderGroups,
                                            m_properties.maxRecursionDepth};
    createInfo.layout = m_shader->getPipelineLayout().current;

    auto result =
        m_device->current.createRayTracingPipelineKHR({}, {}, createInfo);
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
      m_properties{properties}
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