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
class DescriptorPool;
class DescriptorSet;
class Pipeline;
class RaytracePipeline;
class Shader;
class RenderPass;
struct ShaderBindingTable;

class MaterialManager
{
  private:
    struct VertexInputDescription
    {
        std::vector<vk::VertexInputBindingDescription> bindings;
        std::vector<vk::VertexInputAttributeDescription> attributes;
    };

    struct Material
    {
        const Shader *shader;
        const Pipeline *pipeline;
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
    const DescriptorPool *const m_descriptorPool;

    std::vector<const Shader *> m_shaders;
    std::vector<const Pipeline *> m_pipelines;
    std::vector<const ShaderBindingTable *> m_SBTs;

    AllocatedBuffer m_materialDataBuffer;

    int m_currentMaterialIndex = -1;
    std::unordered_map<std::string, uint32_t> m_materialIndexTable;
    std::unordered_map<uint32_t, std::string> m_materialShaderTable;
    std::unordered_map<uint32_t, RasterMaterial> m_rasterMaterials;
    std::unordered_map<uint32_t, RaytraceMaterial> m_raytraceMaterials;
    std::unordered_map<std::string, std::vector<DescriptorSet>>
        m_rasterDescSets;
    std::unordered_map<std::string, std::vector<DescriptorSet>>
        m_raytraceDescSets;

    static vk::Format getFormatFromVertexAttribInfo(
        const scene::VertexInfo &info);
    static VertexInputDescription getVertexInputDescription(
        const scene::RasterPipelineData &rasterData);
    const Shader *createShader(const scene::ShaderData &shaderData);
    const Pipeline *createPipeline(vulkan::ShadingPipeline shadingPipeline,
                                   const RenderPass &renderPass,
                                   const Shader *shader,
                                   const scene::Material &material);
    void createDescriptorSets(const Shader *shader, vulkan::ShadingPipeline pipeline);
    const ShaderBindingTable *createSBT(const RaytracePipeline *pipeline);

  public:
    MaterialManager(const Device *device, const Allocator *allocator,
                    const DescriptorPool *descriptorPool);
    ~MaterialManager();
    MaterialManager(const MaterialManager &materialData) = delete;
    MaterialManager &operator=(const MaterialManager &materialData) = delete;

    int addMaterial(const RenderPass &renderPass,
                    const scene::Material &material);

    inline int getMaterialIndexFromName(const std::string &materialName) const
    {
        return m_materialIndexTable.find(materialName) ==
                       m_materialIndexTable.end()
                   ? -1
                   : static_cast<int>(m_materialIndexTable.at(materialName));
    }

    inline std::string getShaderNameForMaterial(
        const uint32_t materialIndex) const
    {
        return m_materialShaderTable.at(materialIndex);
    }

    [[nodiscard]] inline const Pipeline *getPipeline(
        uint32_t materialIndex, vulkan::ShadingPipeline shadingPipeline)
    {
        return shadingPipeline == vulkan::ShadingPipeline::RASTER
                   ? m_rasterMaterials.at(materialIndex).pipeline
                   : m_raytraceMaterials.at(materialIndex).pipeline;
    }

    [[nodiscard]] inline const std::vector<DescriptorSet> &getDescriptorSets(
        uint32_t materialIndex, vulkan::ShadingPipeline shadingPipeline)
    {
        return shadingPipeline == vulkan::ShadingPipeline::RASTER
                   ? m_rasterDescSets.at(
                         getShaderNameForMaterial(materialIndex))
                   : m_raytraceDescSets.at(
                         getShaderNameForMaterial(materialIndex));
    }

    [[nodiscard]] inline const ShaderBindingTable &getShaderBindingTable(
        uint32_t materialIndex)
    {
        assert(m_raytraceMaterials.at(materialIndex).sbt != nullptr);
        return *m_raytraceMaterials.at(materialIndex).sbt;
    }
};
} // namespace kirana::viewport::vulkan
#endif