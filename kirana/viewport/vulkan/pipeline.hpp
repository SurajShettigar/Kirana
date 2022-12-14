#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include "vulkan_types.hpp"

namespace kirana::viewport::vulkan
{
class Device;
class RenderPass;
class Shader;
class PipelineLayout;
struct VertexInputDescription;

class Pipeline
{
  private:
    bool m_isInitialized = false;
    vk::Pipeline m_current;

    const Device *const m_device;
    const RenderPass *const m_renderPass;
    const Shader *const m_shader;
    const PipelineLayout *const m_pipelineLayout;

    bool build(
        const std::vector<vk::VertexInputBindingDescription> &vertexBindings,
        const std::vector<vk::VertexInputAttributeDescription>
            &vertexAttributes,
        const PipelineProperties &properties);

  public:
    explicit Pipeline(const Device *device, const RenderPass *renderPass,
                      const Shader *shader,
                      const PipelineLayout *pipelineLayout,
                      const VertexInputDescription &vertexInputDesc,
                      const PipelineProperties &pipelineProperties);
    ~Pipeline();
    Pipeline(const Pipeline &pipeline) = delete;
    Pipeline &operator=(const Pipeline &pipeline) = delete;

    const bool &isInitialized = m_isInitialized;
    const vk::Pipeline &current = m_current;

    //    bool rebuild(const VertexInputDescription &vertexInputDesc);
};
} // namespace kirana::viewport::vulkan
#endif