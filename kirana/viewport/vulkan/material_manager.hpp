#ifndef MATERIAL_MANAGER_HPP
#define MATERIAL_MANAGER_HPP

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
class Allocator;
class DescriptorSet;
class Pipeline;
class RaytracePipeline;
class Shader;
class RenderPass;
struct ShaderBindingTable;
class PushConstantBase;

class MaterialManager
{
  private:
    struct VertexInputDescription
    {
        std::vector<vk::VertexInputBindingDescription> bindings;
        std::vector<vk::VertexInputAttributeDescription> attributes;
    };
    struct MaterialDataDescriptor
    {
        const DescriptorSet *set;
        const AllocatedBuffer *buffer;
    };

    struct Material
    {
        const Shader *shader;
        const Pipeline *pipeline;
        PushConstantBase *pushConstantData = nullptr;
    };

    struct RasterMaterial : Material
    {
    };

    struct RaytraceMaterial : Material
    {
        const ShaderBindingTable *sbt = nullptr;
    };

    const Device *const m_device;
    const Allocator *const m_allocator;

    std::vector<const Shader *> m_shaders;
    std::vector<const Pipeline *> m_pipelines;
    std::vector<const ShaderBindingTable *> m_SBTs;

    AllocatedBuffer m_cameraDataBuffer;
    AllocatedBuffer m_worldDataBuffer;
    AllocatedBuffer m_materialDataBuffer;
    AllocatedBuffer m_objectDataBuffer;

    std::unordered_map<std::string, RasterMaterial> m_rasterMaterials;
    std::unordered_map<std::string, RaytraceMaterial> m_raytraceMaterials;

    static vk::Format getFormatFromVertexAttribInfo(
        const scene::VertexInfo &info);
    static VertexInputDescription getVertexInputDescription(
        const scene::RasterPipelineData &rasterData);
    const Shader *createShader(const scene::ShaderData &shaderData);
    const Pipeline *createPipeline(vulkan::ShadingPipeline shadingPipeline,
                                   const RenderPass &renderPass,
                                   const Shader *shader,
                                   const scene::Material &material);
    const ShaderBindingTable *createSBT(const RaytracePipeline *pipeline);

  public:
    MaterialManager(const Device *device, const Allocator *allocator);
    ~MaterialManager();
    MaterialManager(const MaterialManager &materialData) = delete;
    MaterialManager &operator=(const MaterialManager &materialData) = delete;

    bool addMaterial(const RenderPass &renderPass,
                     const scene::Material &material);

    [[nodiscard]] inline const Pipeline *getPipeline(
        const std::string &materialName,
        vulkan::ShadingPipeline shadingPipeline)
    {
        return shadingPipeline == vulkan::ShadingPipeline::RASTER
                   ? m_rasterMaterials.at(materialName).pipeline
                   : m_raytraceMaterials.at(materialName).pipeline;
    }

    [[nodiscard]] inline const ShaderBindingTable &getShaderBindingTable(
        const std::string &materialName)
    {
        assert(m_raytraceMaterials.at(materialName).sbt != nullptr);
        return *m_raytraceMaterials.at(materialName).sbt;
    }
};
} // namespace kirana::viewport::vulkan
#endif