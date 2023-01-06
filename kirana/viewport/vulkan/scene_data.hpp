#ifndef SCENE_DATA_HPP
#define SCENE_DATA_HPP

#include <vector>
#include <unordered_map>
#include <execution>
#include "vulkan_types.hpp"
#include "../viewport_types.hpp"

namespace kirana::scene
{
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
    VertexInputDescription m_vertexDesc;
    mutable std::unordered_map<std::string, std::unique_ptr<Pipeline>>
        m_materials;
    std::unordered_map<std::string, MeshData> m_meshes;
    size_t m_totalInstanceCount;
    CameraData m_cameraData;
    AllocatedBuffer m_cameraBuffer;
    AllocatedBuffer m_worldDataBuffer;
    AllocatedBuffer m_objectBuffer;
    // Raytracing data
    AccelerationStructure *m_accelStructure = nullptr;
    RaytracePipeline *m_raytracePipeline = nullptr;
    ShaderBindingTable *m_shaderBindingTable = nullptr;

    const Device *const m_device;
    const Allocator *const m_allocator;
    const RenderPass *m_renderPass;
    const DescriptorSetLayout *m_globalDescSetLayout;
    const DescriptorSetLayout *m_objectDescSetLayout;
    const DescriptorSetLayout *m_raytraceDescSetLayout;

    const scene::ViewportScene &m_scene;
    uint32_t m_cameraChangeListener;
    uint32_t m_worldChangeListener;

    void setVertexDescription();

    void onWorldChanged();
    void onCameraChanged();
    void onObjectChanged();

    void createWorldDataBuffer();
    void createCameraBuffer();

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

    [[nodiscard]] const vk::AccelerationStructureKHR &getAccelerationStructure()
        const;

    [[nodiscard]] inline const std::unordered_map<std::string, MeshData>
        &getMeshData() const
    {
        return m_meshes;
    }

    inline bool shouldRenderOutline() const
    {
        return m_currentShading == viewport::Shading::BASIC;
    }

    [[nodiscard]] const Pipeline *getOutlineMaterial() const;

    inline void setShading(viewport::Shading shading)
    {
        m_currentShading = shading;
    };
    inline viewport::Shading getCurrentShading() const
    {
        return m_currentShading;
    }

    [[nodiscard]] const scene::WorldData &getWorldData() const;

    void rebuildPipeline(const RenderPass *renderPass);

    [[nodiscard]] const AllocatedBuffer &getCameraBuffer() const;
    [[nodiscard]] uint32_t getCameraBufferOffset(uint32_t offsetIndex) const;

    [[nodiscard]] const AllocatedBuffer &getWorldDataBuffer() const;
    [[nodiscard]] uint32_t getWorldDataBufferOffset(uint32_t offsetIndex) const;

    [[nodiscard]] const AllocatedBuffer &getObjectBuffer() const;
    [[nodiscard]] uint32_t getObjectBufferOffset(uint32_t offsetIndex) const;
};
} // namespace kirana::viewport::vulkan
#endif