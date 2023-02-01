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
typedef uint32_t INDEX_TYPE;
} // namespace kirana::scene

namespace kirana::viewport::vulkan
{
class Device;
class Allocator;
class DescriptorPool;
class RenderPass;
class RaytraceData;
class MaterialManager;

class SceneData
{
  private:
    struct BatchBufferData
    {
        AllocatedBuffer cpuBuffer;
        AllocatedBuffer gpuBuffer;
        uint32_t currentDataOffset;
        size_t currentSize;
        char *currentMemoryPointer;
    };
    bool m_isInitialized = false;
    const Device *const m_device;
    const Allocator *const m_allocator;
    const DescriptorPool *const m_descriptorPool;
    const RenderPass *m_renderPass;
    const RaytraceData *const m_raytraceData;
    const scene::ViewportScene &m_scene;
    vulkan::ShadingPipeline m_currentShadingPipeline =
        vulkan::ShadingPipeline::RASTER;
    vulkan::ShadingType m_currentShadingType = vulkan::ShadingType::BASIC;
    uint32_t m_raytracedFrameCount = 0;
    size_t m_totalInstanceCount = 0;

    MaterialManager *m_materialManager = nullptr;

    std::vector<MeshData> m_editorMeshes;
    std::vector<MeshData> m_sceneMeshes;

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

    void createWorldDataBuffer();
    void createCameraBuffer();

    bool transferBufferToGPU(BatchBufferData &bufferData);
    std::pair<int, int> createVertexAndIndexBuffer(
        const std::vector<scene::Vertex> &vertices,
        const std::vector<scene::INDEX_TYPE> &indices);


    void createEditorMaterials();
    void createSceneMaterials();
    static bool hasMeshData(const std::vector<MeshData> &meshes,
                            const std::string &meshName, uint32_t *meshIndex);
    bool createMeshes(bool isEditor = false);

  public:
    SceneData(
        const Device *device, const Allocator *allocator, const DescriptorPool *descriptorPool,
        const RenderPass *renderPass, const RaytraceData *raytraceData,
        const scene::ViewportScene &scene,
        vulkan::ShadingPipeline pipeline = vulkan::ShadingPipeline::RASTER,
        vulkan::ShadingType type = vulkan::ShadingType::BASIC);
    ~SceneData();

    SceneData(const SceneData &sceneData) = delete;

    const bool &isInitialized = m_isInitialized;

    inline void setShadingPipeline(vulkan::ShadingPipeline pipeline)
    {
        m_currentShadingPipeline = pipeline;
    };
    [[nodiscard]] inline vulkan::ShadingPipeline getCurrentShadingPipeline()
        const
    {
        return m_currentShadingPipeline;
    }
    inline void setShadingType(vulkan::ShadingType type)
    {
        m_currentShadingType = type;
    };
    [[nodiscard]] inline vulkan::ShadingType getCurrentShadingType() const
    {
        return m_currentShadingType;
    }

    [[nodiscard]] inline const std::vector<MeshData> &getEditorMeshes() const
    {
        return m_editorMeshes;
    }

    [[nodiscard]] inline const std::vector<MeshData> &getSceneMeshes() const
    {
        return m_sceneMeshes;
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

    [[nodiscard]] inline vk::DeviceAddress getVertexBufferAddress(
        int bufferIndex) const
    {
        return bufferIndex == -1
                   ? 0
                   : m_vertexBuffers[bufferIndex].gpuBuffer.address;
    }

    [[nodiscard]] inline vk::DeviceAddress getIndexBufferAddress(
        int bufferIndex) const
    {
        return bufferIndex == -1
                   ? 0
                   : m_indexBuffers[bufferIndex].gpuBuffer.address;
    }

    [[nodiscard]] PushConstantRaster getPushConstantRasterData(
        bool isEditor, uint32_t meshIndex, uint32_t instanceIndex) const;
    [[nodiscard]] PushConstantRaytrace getPushConstantRaytraceData() const;
};
} // namespace kirana::viewport::vulkan
#endif