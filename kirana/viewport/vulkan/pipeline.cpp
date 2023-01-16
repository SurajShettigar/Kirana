#include "pipeline.hpp"
#include "device.hpp"
#include "renderpass.hpp"
#include "pipeline_layout.hpp"
#include "vulkan_utils.hpp"
#include "push_constant.hpp"

kirana::viewport::vulkan::Pipeline::Pipeline(
    const Device *const device, const RenderPass *const renderPass,
    const std::vector<const DescriptorSetLayout *> &descriptorSetLayouts,
    const std::vector<const PushConstantBase *> &pushConstants,
    std::string name, std::string shaderName)
    : m_isInitialized{false}, m_device{device}, m_renderPass{renderPass},
      m_pipelineLayout{
          new PipelineLayout(m_device, descriptorSetLayouts, pushConstants)},
      m_name{std::move(name)}, m_shaderName{std::move(shaderName)}
{
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
