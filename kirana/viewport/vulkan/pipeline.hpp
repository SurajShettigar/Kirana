#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include "vulkan_types.hpp"

namespace kirana::viewport::vulkan
{
class Device;
class RenderPass;
class Shader;

class Pipeline
{
  protected:
    bool m_isInitialized = false;

    const Device *const m_device;
    const RenderPass *const m_renderPass;
    const Shader *const m_shader;

    std::string m_name = "MatCap";
    vulkan::ShadingPipeline m_shadingPipeline = vulkan::ShadingPipeline::RASTER;

    vk::Pipeline m_current;

    virtual bool build() = 0;

  public:
    explicit Pipeline(const Device *device, const RenderPass *renderPass,
                      const Shader *shader,
                      vulkan::ShadingPipeline shadingPipeline);
    virtual ~Pipeline();
    Pipeline(const Pipeline &pipeline) = delete;
    Pipeline &operator=(const Pipeline &pipeline) = delete;

    const bool &isInitialized = m_isInitialized;
    const std::string &name = m_name;
    const vulkan::ShadingPipeline &shadingPipeline = m_shadingPipeline;

    const vk::Pipeline &current = m_current;
};
} // namespace kirana::viewport::vulkan
#endif