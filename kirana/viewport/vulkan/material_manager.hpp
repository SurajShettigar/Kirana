#ifndef MATERIAL_MANAGER_HPP
#define MATERIAL_MANAGER_HPP

#include "vulkan_types.hpp"

namespace kirana::scene
{
struct VertexInfo;
class Material;
struct ShaderData;
struct RasterPipelineData;
struct MaterialDataBase;

} // namespace kirana::scene

namespace kirana::viewport::vulkan
{
class Device;
class Allocator;
class Pipeline;
class RaytracePipeline;
class Shader;
class RenderPass;
class ShaderBindingTable;

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
        std::vector<int> shaderIndices;
        std::vector<int> pipelineIndices;
        int dataBufferIndex = -1; // Index used to fetch appropriate data buffer
        int materialDataIndex = -1; // Index local to data buffer
        int sbtIndex = -1;
    };

    const Device *const m_device;
    const Allocator *const m_allocator;

    std::vector<const Shader *> m_shaders;
    std::vector<const Pipeline *> m_pipelines;
    std::vector<const ShaderBindingTable *> m_SBTs;

    std::unordered_map<std::string, std::string> m_materialShaderTable;
    std::unordered_map<std::string, uint32_t> m_materialIndexTable;
    std::vector<Material> m_materials;
    std::unordered_map<std::string, std::vector<BatchBufferData>>
        m_materialDataBuffers;

    static vk::Format getFormatFromVertexAttribInfo(
        const scene::VertexInfo &info);
    static VertexInputDescription getVertexInputDescription(
        const scene::RasterPipelineData &rasterData);

    int createShader(const scene::ShaderData &shaderData);
    int createPipeline(vulkan::ShadingPipeline shadingPipeline,
                       const RenderPass &renderPass, const Shader *shader,
                       const scene::Material &material);
    int copyMaterialDataToBuffer(const scene::Material &material);
    int createSBT(const RaytracePipeline *pipeline);

  public:
    MaterialManager(const Device *device, const Allocator *allocator);
    ~MaterialManager();
    MaterialManager(const MaterialManager &materialData) = delete;
    MaterialManager &operator=(const MaterialManager &materialData) = delete;

    uint32_t addMaterial(const RenderPass &renderPass,
                         const scene::Material &material);

    inline int getMaterialIndexFromName(const std::string &materialName) const
    {
        return m_materialIndexTable.find(materialName) ==
                       m_materialIndexTable.end()
                   ? -1
                   : static_cast<int>(m_materialIndexTable.at(materialName));
    }

    inline std::string getShaderNameForMaterial(
        const std::string &materialName) const
    {
        return m_materialShaderTable.at(materialName);
    }

    [[nodiscard]] vk::DeviceAddress getMaterialDataBufferAddress(
        uint32_t materialIndex) const;
    [[nodiscard]] inline int getMaterialDataIndex(uint32_t materialIndex) const
    {
        return m_materials[materialIndex].materialDataIndex;
    }

    [[nodiscard]] inline const Pipeline *getPipeline(
        uint32_t materialIndex, vulkan::ShadingPipeline shadingPipeline)
    {
        // TODO: Return default pipeline if none exists
        return m_pipelines[m_materials[materialIndex].pipelineIndices
                               [static_cast<int>(shadingPipeline)]];
    }

    [[nodiscard]] inline const ShaderBindingTable *getShaderBindingTable(
        uint32_t materialIndex)
    {
        // TODO: Return default SBT if none exists
        return m_SBTs[m_materials[materialIndex].sbtIndex];
    }
};
} // namespace kirana::viewport::vulkan
#endif