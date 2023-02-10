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
    std::vector<vk::PipelineShaderStageCreateInfo> m_shaderStages;
    std::vector<vk::RayTracingShaderGroupCreateInfoKHR> m_shaderGroups;

    bool build() override;

  public:
    explicit RaytracePipeline(const Device *device,
                              const RenderPass *renderPass,
                              const Shader *shader, Properties properties);
    ~RaytracePipeline() override;

    RaytracePipeline(const RaytracePipeline &pipeline) = delete;
    RaytracePipeline &operator=(const RaytracePipeline &pipeline) = delete;

    [[nodiscard]] inline const std::vector<vk::PipelineShaderStageCreateInfo>
        &getShaderStagesInfo() const
    {
        return m_shaderStages;
    }

    [[nodiscard]] inline const std::vector<
        vk::RayTracingShaderGroupCreateInfoKHR>
        &getShaderGroupsInfo() const
    {
        return m_shaderGroups;
    }
};
} // namespace kirana::viewport::vulkan
#endif