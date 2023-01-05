#ifndef SCENE_DATA_HPP
#define SCENE_DATA_HPP

#include <vector>
#include <unordered_map>
#include <execution>
#include "vulkan_types.hpp"
#include "../viewport_types.hpp"

namespace kirana::scene
{
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
class SceneData
{
  private:
    bool m_isInitialized = false;
    viewport::Shading m_currentShading = viewport::Shading::BASIC;
    VertexInputDescription m_vertexDesc;
    mutable std::unordered_map<std::string, MaterialData> m_materials;
    std::unordered_map<std::string, MeshData> m_meshes;
    size_t m_totalInstanceCount;
    CameraData m_cameraData;
    AllocatedBuffer m_cameraBuffer;
    AllocatedBuffer m_worldDataBuffer;
    AllocatedBuffer m_objectBuffer;
    AccelerationStructure *m_accelStructure;

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

    MaterialData getMaterialData(const scene::Material &material);
    void createMaterials();
    MaterialData &findMaterial(const std::string &materialName,
                               bool overrideShading = false);

    bool createMeshes();
    bool createObjectBuffer();

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

    [[nodiscard]] const vk::AccelerationStructureKHR &
    getAccelerationStructure() const;

    [[nodiscard]] inline const std::unordered_map<std::string, MeshData>
        &getMeshData() const
    {
        return m_meshes;
    }

    inline bool shouldRenderOutline() const
    {
        return m_currentShading == viewport::Shading::BASIC;
    }
    inline bool shouldRenderOnlyVisible() const
    {
        return m_currentShading == viewport::Shading::RAYTRACE_PBR;
    }
    [[nodiscard]] const MaterialData &getOutlineMaterial() const;

    void setShading(viewport::Shading shading);
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