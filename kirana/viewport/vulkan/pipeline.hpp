#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include "vulkan_types.hpp"

namespace kirana::viewport::vulkan
{
class Device;
class RenderPass;
class DescriptorSetLayout;
class Shader;
class PipelineLayout;
struct VertexInputDescription;

class Pipeline
{
  protected:
    bool m_isInitialized = false;

    const Device *const m_device;
    const RenderPass *const m_renderPass;

    PipelineLayout *m_pipelineLayout;
    std::string m_name = "Material";
    std::string m_shaderName = "MatCap";

    vk::Pipeline m_current;

    virtual bool build() = 0;

  public:
    explicit Pipeline(
        const Device *device, const RenderPass *renderPass,
        const std::vector<const DescriptorSetLayout *> &descriptorSetLayouts,
        std::string name, std::string shaderName);
    virtual ~Pipeline();
    Pipeline(const Pipeline &pipeline) = delete;
    Pipeline &operator=(const Pipeline &pipeline) = delete;

    const bool &isInitialized = m_isInitialized;
    const std::string &name = m_name;
    const std::string &shaderName = m_shaderName;

    const vk::Pipeline &current = m_current;

    [[nodiscard]] inline const PipelineLayout &getLayout() const
    {
        return *m_pipelineLayout;
    }
};
} // namespace kirana::viewport::vulkan
#endif