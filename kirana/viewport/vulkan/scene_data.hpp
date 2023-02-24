#ifndef SCENE_DATA_HPP
#define SCENE_DATA_HPP

#include <vector>
#include <unordered_map>
#include "vulkan_types.hpp"

namespace kirana::scene
{
struct Vertex;
struct CameraData;
struct WorldData;
class ViewportScene;
class Material;
struct SceneInfo;
struct Renderable;
class Mesh;
typedef uint32_t INDEX_TYPE;
} // namespace kirana::scene

namespace kirana::viewport::vulkan
{
class Device;
class Allocator;
class DescriptorPool;
class RenderPass;
class RaytraceData;
class ShaderBindingTable;
class MaterialManager;
class PipelineLayout;
template <typename> class PushConstant;
class SceneData
{
  private:
    mutable utils::Event<> m_onSceneDataChange;

    bool m_isInitialized = false;
    bool m_isRaytracingInitialized = false;
    const Device *const m_device;
    const Allocator *const m_allocator;
    const DescriptorPool *const m_descriptorPool;
    const RenderPass *m_renderPass;
    RaytraceData *const m_raytraceData;
    const scene::ViewportScene &m_scene;

    // TODO: Switch to per-shader pipeline layout using shader reflection.
    const PipelineLayout *m_rasterPipelineLayout = nullptr;
    // TODO: Switch to per-shader descriptor set using shader reflection.
    std::vector<DescriptorSet> m_rasterDescSets;

    vulkan::ShadingPipeline m_currentShadingPipeline =
        vulkan::ShadingPipeline::RASTER;
    vulkan::ShadingType m_currentShadingType = vulkan::ShadingType::BASIC;

    MaterialManager *m_materialManager = nullptr;

    std::vector<MeshObjectData> m_editorMeshes;
    std::vector<MeshObjectData> m_sceneMeshes;

    AllocatedBuffer m_cameraBuffer;
    AllocatedBuffer m_worldDataBuffer;
    AllocatedBuffer m_objectDataBuffer;
    std::vector<BatchBufferData> m_vertexBuffers;
    std::vector<BatchBufferData> m_indexBuffers;

    uint32_t m_cameraChangeListener;
    uint32_t m_worldChangeListener;
    uint32_t m_sceneLoadListener;

    void onWorldChanged();
    void onCameraChanged();
    void onSceneLoaded(bool result);
    void onObjectChanged();

    void createWorldDataBuffer();
    void createCameraBuffer();

    std::pair<int, int> createVertexAndIndexBuffer(
        const std::vector<scene::Vertex> &vertices,
        const std::vector<scene::INDEX_TYPE> &indices);


    void createEditorMaterials();
    void createSceneMaterials();
    static int getMeshObject(
        const std::vector<MeshObjectData> &meshObjects,
        const std::vector<std::shared_ptr<scene::Mesh>> &meshes);
    bool createMeshes(bool isEditor = false);
    void createObjectBuffer();

  public:
    SceneData(
        const Device *device, const Allocator *allocator,
        const DescriptorPool *descriptorPool, const RenderPass *renderPass,
        RaytraceData *raytraceData, const scene::ViewportScene &scene,
        vulkan::ShadingPipeline pipeline = vulkan::ShadingPipeline::RASTER,
        vulkan::ShadingType type = vulkan::ShadingType::BASIC);
    ~SceneData();

    SceneData(const SceneData &sceneData) = delete;

    const bool &isInitialized = m_isInitialized;
    const bool &isRaytracingInitialized = m_isRaytracingInitialized;

    [[nodiscard]] inline uint32_t addOnSceneDataChangeListener(
        const std::function<void()> &callback) const
    {
        return m_onSceneDataChange.addListener(callback);
    }

    inline void removeOnSceneDataChangeListener(uint32_t callbackId) const
    {
        m_onSceneDataChange.removeListener(callbackId);
    }

    [[nodiscard]] inline const RaytraceData &getRaytraceData() const
    {
        return *m_raytraceData;
    }

    [[nodiscard]] inline const PipelineLayout &getRasterPipelineLayout() const
    {
        return *m_rasterPipelineLayout;
    }

    [[nodiscard]] inline const std::vector<DescriptorSet>
        &getRasterDescriptorSets() const
    {
        return m_rasterDescSets;
    }

    [[nodiscard]] inline vulkan::ShadingPipeline getCurrentShadingPipeline()
        const
    {
        return m_currentShadingPipeline;
    }
    [[nodiscard]] inline vulkan::ShadingType getCurrentShadingType() const
    {
        return m_currentShadingType;
    }
    void setShadingPipeline(vulkan::ShadingPipeline pipeline);
    void setShadingType(vulkan::ShadingType type);

    [[nodiscard]] inline const std::vector<MeshObjectData> &getEditorMeshes()
        const
    {
        return m_editorMeshes;
    }

    [[nodiscard]] inline const std::vector<MeshObjectData> &getSceneMeshes()
        const
    {
        return m_sceneMeshes;
    }

    [[nodiscard]] uint32_t getCurrentMaterialIndex(bool isEditorMesh,
                                                   bool outline,
                                                   uint32_t objIndex,
                                                   uint32_t meshIndex) const;

    const Pipeline &getCurrentPipeline(bool isEditorMesh, bool outline,
                                       uint32_t objIndex,
                                       uint32_t meshIndex) const;

    const ShaderBindingTable &getCurrentSBT(uint32_t objIndex, uint32_t meshIndex) const;

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


    [[nodiscard]] inline const AllocatedBuffer &getObjectDataBuffer() const
    {
        return m_objectDataBuffer;
    }

    inline const vk::Buffer &getVertexBuffer(bool isEditorMesh,
                                             uint32_t objectIndex,
                                             uint32_t meshIndex) const
    {
        return isEditorMesh ? *m_vertexBuffers[m_editorMeshes[objectIndex]
                                                   .meshes[meshIndex]
                                                   .vertexBufferIndex]
                                   .buffer.buffer
                            : *m_vertexBuffers[m_sceneMeshes[objectIndex]
                                                   .meshes[meshIndex]
                                                   .vertexBufferIndex]
                                   .buffer.buffer;
    }

    inline const vk::Buffer &getIndexBuffer(bool isEditorMesh,
                                            uint32_t objectIndex,
                                            uint32_t meshIndex) const
    {
        return isEditorMesh ? *m_indexBuffers[m_editorMeshes[objectIndex]
                                                  .meshes[meshIndex]
                                                  .indexBufferIndex]
                                   .buffer.buffer
                            : *m_indexBuffers[m_sceneMeshes[objectIndex]
                                                  .meshes[meshIndex]
                                                  .indexBufferIndex]
                                   .buffer.buffer;
    }

    [[nodiscard]] inline vk::DeviceAddress getVertexBufferAddress(
        int bufferIndex) const
    {
        return bufferIndex == -1 ? 0
                                 : m_vertexBuffers[bufferIndex].buffer.address;
    }

    [[nodiscard]] inline vk::DeviceAddress getIndexBufferAddress(
        int bufferIndex) const
    {
        return bufferIndex == -1 ? 0
                                 : m_indexBuffers[bufferIndex].buffer.address;
    }

    [[nodiscard]] PushConstant<PushConstantRaster> getPushConstantRasterData(
        bool isEditor, bool outline,uint32_t objIndex, uint32_t meshIndex,
        uint32_t instanceIndex) const;
    [[nodiscard]] PushConstant<PushConstantRaytrace>
    getPushConstantRaytraceData() const;
};
} // namespace kirana::viewport::vulkan
#endif