#include "pipeline.hpp"
#include "device.hpp"
#include "renderpass.hpp"
#include "shader.hpp"
#include "vulkan_utils.hpp"

kirana::viewport::vulkan::Pipeline::Pipeline(
    const Device *const device, const RenderPass *const renderPass,
    const Shader *const shader, vulkan::ShadingPipeline shadingPipeline)
    : m_isInitialized{false}, m_device{device}, m_renderPass{renderPass},
      m_shader{shader}, m_name{m_shader->name}, m_shadingPipeline{
                                                    shadingPipeline}
{
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
