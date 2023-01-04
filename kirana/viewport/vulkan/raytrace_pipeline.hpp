#ifndef RAYTRACE_PIPELINE_HPP
#define RAYTRACE_PIPELINE_HPP

#include "pipeline.hpp"

namespace kirana::viewport::vulkan
{
class RaytracePipeline : public Pipeline
{
  protected:
    std::vector<vk::RayTracingShaderGroupCreateInfoKHR> m_shaderGroups;

    bool build() override;

  public:
    explicit RaytracePipeline(const Device *device,
                              const RenderPass *renderPass,
                              const Shader *shader,
                              const PipelineLayout *pipelineLayout);
    ~RaytracePipeline() override;

    RaytracePipeline(const RaytracePipeline &pipeline) = delete;
    RaytracePipeline &operator=(const RaytracePipeline &pipeline) = delete;
};
} // namespace kirana::viewport::vulkan
#endif