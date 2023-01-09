#ifndef SCENE_DATA_HPP
#define SCENE_DATA_HPP

#include <vector>
#include <unordered_map>
#include <execution>
#include "vulkan_types.hpp"
#include "../viewport_types.hpp"

namespace kirana::scene
{
struct Vertex;
struct WorldData;
class ViewportScene;
class Material;
struct Renderable;
} // namespace kirana::scene

namespace kirana::viewport::vulkan
{
class Shader;
class Device;
class RenderPass;
class Allocator;
class DescriptorSetLayout;
class AccelerationStructure;
class RaytracePipeline;
class ShaderBindingTable;
class SceneData
{
  private:
    bool m_isInitialized = false;
    viewport::Shading m_currentShading = viewport::Shading::BASIC;

    const Device *const m_device;
    const Allocator *const m_allocator;
    const RenderPass *m_renderPass;
    const DescriptorSetLayout *m_globalDescSetLayout;
    const DescriptorSetLayout *m_objectDescSetLayout;
    const DescriptorSetLayout *m_raytraceDescSetLayout;

    VertexInputDescription m_vertexDesc;
    mutable std::unordered_map<std::string, std::unique_ptr<Pipeline>>
        m_materials;
    std::unordered_map<std::string, MeshData> m_meshes;
    size_t m_totalInstanceCount;
    CameraData m_cameraData;
    AllocatedBuffer m_cameraBuffer;
    AllocatedBuffer m_worldDataBuffer;
    AllocatedBuffer m_vertexBuffer;
    AllocatedBuffer m_indexBuffer;
    AllocatedBuffer m_objectBuffer;
    // Raytracing data
    AccelerationStructure *m_accelStructure = nullptr;
    RaytracePipeline *m_raytracePipeline = nullptr;
    ShaderBindingTable *m_shaderBindingTable = nullptr;
    RaytracedGlobalData m_raytracedGlobalData;
    AllocatedBuffer m_raytracedGlobalBuffer;
    std::vector<RaytracedObjectData> m_raytracedObjectData;
    AllocatedBuffer m_raytracedObjectBuffer;

    const scene::ViewportScene &m_scene;
    uint32_t m_cameraChangeListener;
    uint32_t m_worldChangeListener;

    void setVertexDescription();

    void onWorldChanged();
    void onCameraChanged();
    void onObjectChanged();

    void createWorldDataBuffer();
    void createCameraBuffer();
    bool createVertexAndIndexBuffer(const std::vector<scene::Vertex> &vertices,
                                    const std::vector<uint32_t> &indices);

    std::unique_ptr<Pipeline> getPipelineForMaterial(
        const scene::Material &material);
    void createMaterials();
    const std::unique_ptr<Pipeline> &findMaterial(
        const std::string &materialName, bool overrideShading = false);

    bool createMeshes();
    bool createObjectBuffer();
    bool initializeRaytracing();

  public:
    SceneData(const Device *device, const Allocator *allocator,
              const RenderPass *renderPass, const scene::ViewportScene &scene,
              viewport::Shading shading = viewport::Shading::BASIC);
    ~SceneData();

    SceneData(const SceneData &sceneData) = delete;

    const bool &isInitialized = m_isInitialized;

    inline bool shouldRenderOutline() const
    {
        return m_currentShading == viewport::Shading::BASIC;
    }

    inline void setShading(viewport::Shading shading)
    {
        m_currentShading = shading;
    };
    inline viewport::Shading getCurrentShading() const
    {
        return m_currentShading;
    }

    [[nodiscard]] inline const DescriptorSetLayout *
    getGlobalDescriptorSetLayout() const
    {
        return m_globalDescSetLayout;
    }

    [[nodiscard]] inline const DescriptorSetLayout *
    getObjectDescriptorSetLayout() const
    {
        return m_objectDescSetLayout;
    }

    [[nodiscard]] inline const DescriptorSetLayout *
    getRaytraceDescriptorSetLayout() const
    {
        return m_raytraceDescSetLayout;
    }

    [[nodiscard]] inline const std::unordered_map<std::string, MeshData>
        &getMeshData() const
    {
        return m_meshes;
    }

    [[nodiscard]] const Pipeline *getOutlineMaterial() const;

    [[nodiscard]] const scene::WorldData &getWorldData() const;

    [[nodiscard]] inline const AllocatedBuffer &getCameraBuffer() const
    {
        return m_cameraBuffer;
    }
    [[nodiscard]] uint32_t getCameraBufferOffset(uint32_t offsetIndex) const;

    [[nodiscard]] inline const AllocatedBuffer &getWorldDataBuffer() const
    {
        return m_worldDataBuffer;
    }
    [[nodiscard]] uint32_t getWorldDataBufferOffset(uint32_t offsetIndex) const;

    [[nodiscard]] inline const AllocatedBuffer &getVertexBuffer() const
    {
        return m_vertexBuffer;
    }
    [[nodiscard]] inline const AllocatedBuffer &getIndexBuffer() const
    {
        return m_indexBuffer;
    }

    [[nodiscard]] inline const AllocatedBuffer &getObjectBuffer() const
    {
        return m_objectBuffer;
    }
    [[nodiscard]] uint32_t getObjectBufferOffset(uint32_t offsetIndex) const;

    [[nodiscard]] const vk::AccelerationStructureKHR &getAccelerationStructure()
        const;

    [[nodiscard]] inline const RaytracePipeline &getRaytracePipeline() const
    {
        return *m_raytracePipeline;
    }

    [[nodiscard]] inline const ShaderBindingTable &getShaderBindingTable() const
    {
        return *m_shaderBindingTable;
    }

    [[nodiscard]] inline const AllocatedBuffer &getRaytracedGlobalBuffer() const
    {
        return m_raytracedGlobalBuffer;
    }

    [[nodiscard]] inline const AllocatedBuffer &getRaytracedObjectBuffer() const
    {
        return m_raytracedObjectBuffer;
    }
};
} // namespace kirana::viewport::vulkan
#endif