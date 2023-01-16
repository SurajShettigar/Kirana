#ifndef RAYTRACE_PIPELINE_HPP
#define RAYTRACE_PIPELINE_HPP

#include "pipeline.hpp"

namespace kirana::viewport::vulkan
{
class RaytracePipeline : public Pipeline
{
  protected:
    uint32_t m_maxRecursionDepth = 2;
    bool build() override;

  public:
    explicit RaytracePipeline(const Device *device, const RenderPass *renderPass,
                              const std::vector<const DescriptorSetLayout *> &descriptorSetLayouts,
                              const std::vector<const PushConstantBase *> &pushConstants,
                              std::string name, std::string shaderName);
    ~RaytracePipeline() override;

    RaytracePipeline(const RaytracePipeline &pipeline) = delete;
    RaytracePipeline &operator=(const RaytracePipeline &pipeline) = delete;
};
} // namespace kirana::viewport::vulkan
#endif