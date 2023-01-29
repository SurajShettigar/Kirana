#ifndef RAYTRACE_PIPELINE_HPP
#define RAYTRACE_PIPELINE_HPP

#include "pipeline.hpp"

namespace kirana::viewport::vulkan
{
class RaytracePipeline : public Pipeline
{
  public:
    struct Properties
    {
        uint32_t maxRecursionDepth = 2;
    };

  protected:
    Properties m_properties{};
    vk::RayTracingPipelineCreateInfoKHR m_createInfo{};

    bool build() override;

  public:
    explicit RaytracePipeline(const Device *device,
                              const RenderPass *renderPass,
                              const Shader *shader,
                              Properties properties);
    ~RaytracePipeline() override;

    RaytracePipeline(const RaytracePipeline &pipeline) = delete;
    RaytracePipeline &operator=(const RaytracePipeline &pipeline) = delete;

    [[nodiscard]] inline const vk::RayTracingPipelineCreateInfoKHR &
    getCreateInfo() const
    {
        return m_createInfo;
    }
};
} // namespace kirana::viewport::vulkan
#endif