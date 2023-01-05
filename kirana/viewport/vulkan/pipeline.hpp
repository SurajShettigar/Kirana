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
  public:
    struct Properties
    {
        vk::PrimitiveTopology primitiveType =
            vk::PrimitiveTopology::eTriangleList;
        vk::PolygonMode polygonMode = vk::PolygonMode::eFill;
        vk::CullModeFlags cullMode = vk::CullModeFlagBits::eBack;
        float lineWidth = 1.0f;
        vk::SampleCountFlagBits msaaLevel = vk::SampleCountFlagBits::e1;
        bool alphaBlending = false;
        bool enableDepth = true;
        bool writeDepth = true;
        vk::CompareOp depthCompareOp = vk::CompareOp::eLessOrEqual;
        bool stencilTest = false;
        vk::CompareOp stencilCompareOp = vk::CompareOp::eAlways;
        vk::StencilOp stencilFailOp = vk::StencilOp::eReplace;
        vk::StencilOp stencilDepthFailOp = vk::StencilOp::eReplace;
        vk::StencilOp stencilPassOp = vk::StencilOp::eReplace;
        uint32_t stencilReference = 1;
    };

  protected:
    bool m_isInitialized = false;

    const Device *const m_device;
    const RenderPass *const m_renderPass;

    PipelineLayout *m_pipelineLayout;
    std::string m_name = "Material";
    std::string m_shaderName = "MatCap";

    vk::Pipeline m_current;

    const VertexInputDescription m_vertexInputDesc;
    const Properties m_properties;

    virtual bool build();

  public:
    explicit Pipeline(
        const Device *device, const RenderPass *renderPass,
        const std::vector<const DescriptorSetLayout *> &descriptorSetLayouts,
        std::string name, std::string shaderName,
        VertexInputDescription vertexInputDesc, Properties properties);
    virtual ~Pipeline();
    Pipeline(const Pipeline &pipeline) = delete;
    Pipeline &operator=(const Pipeline &pipeline) = delete;

    const bool &isInitialized = m_isInitialized;
    const vk::Pipeline &current = m_current;

    [[nodiscard]] inline const Properties &getProperties() const
    {
        return m_properties;
    }

    [[nodiscard]] inline const PipelineLayout &getLayout() const
    {
        return *m_pipelineLayout;
    }
};
} // namespace kirana::viewport::vulkan
#endif