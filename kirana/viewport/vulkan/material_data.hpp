#ifndef MATERIAL_DATA_HPP
#define MATERIAL_DATA_HPP

#include "vulkan_types.hpp"

namespace kirana::scene
{
struct VertexInfo;
class Material;
struct ShaderData;
struct RasterPipelineData;

} // namespace kirana::scene

namespace kirana::viewport::vulkan
{
class Device;
class Pipeline;
class Shader;
class RenderPass;
class MaterialData
{
  private:
    struct VertexInputDescription
    {
        std::vector<vk::VertexInputBindingDescription> bindings;
        std::vector<vk::VertexInputAttributeDescription> attributes;
    };
    struct Material
    {
        std::unordered_map<vulkan::ShadingPipeline, const Shader *> shaders;
        std::unordered_map<vulkan::ShadingPipeline, const Pipeline *> pipelines;
    };

    const Device *const m_device;
    std::vector<const Shader *> m_shaders;
    std::vector<const Pipeline *> m_pipelines;
    std::unordered_map<std::string, Material> m_materials;

    static vk::Format getFormatFromVertexAttribInfo(
        const scene::VertexInfo &info);
    static VertexInputDescription getVertexInputDescription(
        const scene::RasterPipelineData &rasterData);
    const Shader *createShader(const scene::ShaderData &shaderData);
    const Pipeline *createPipeline(vulkan::ShadingPipeline shadingPipeline,
                                   const RenderPass &renderPass,
                                   const Shader *shader,
                                   const scene::Material &material);

  public:
    MaterialData(const Device *device);
    ~MaterialData();
    MaterialData(const MaterialData &materialData) = delete;
    MaterialData &operator=(const MaterialData &materialData) = delete;

    bool addMaterial(const RenderPass &renderPass,
                     const scene::Material &material);

    [[nodiscard]] inline const Pipeline &getPipeline(
        const std::string &materialName,
        vulkan::ShadingPipeline shadingPipeline)
    {
        return *m_materials.at(materialName).pipelines.at(shadingPipeline);
    }
};
} // namespace kirana::viewport::vulkan
#endif