#ifndef RASTER_PIPELINE_HPP
#define RASTER_PIPELINE_HPP

#include "pipeline.hpp"

namespace kirana::viewport::vulkan
{
class RasterPipeline : public Pipeline
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
    const VertexInputDescription m_vertexInputDesc{};
    const Properties m_properties{};

    bool build() override;

  public:
    explicit RasterPipeline(
        const Device *device, const RenderPass *renderPass,
        const std::vector<const DescriptorSetLayout *> &descriptorSetLayouts,
        std::string name, std::string shaderName,
        VertexInputDescription vertexInputDesc, Properties properties);
    ~RasterPipeline() override;

    RasterPipeline(const RasterPipeline &pipeline) = delete;
    RasterPipeline &operator=(const RasterPipeline &pipeline) = delete;

    [[nodiscard]] inline const Properties &getProperties() const
    {
        return m_properties;
    }
};
} // namespace kirana::viewport::vulkan
#endif