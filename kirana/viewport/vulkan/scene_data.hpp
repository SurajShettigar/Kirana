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
class Shader;
class Device;
class RenderPass;
class Allocator;
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
    std::vector<BatchBufferData> m_vertexBuffers;
    std::vector<BatchBufferData> m_indexBuffers;
    AllocatedBuffer m_objectBuffer;
    // Raytracing data
    AllocatedBuffer m_raytracedObjectBuffer;

    uint32_t m_cameraChangeListener;
    uint32_t m_worldChangeListener;
    uint32_t m_sceneLoadListener;

    void onWorldChanged();
    void onCameraChanged();
    void onSceneLoaded(bool result);
    void onObjectChanged();

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
    bool createObjectBuffer();

  public:
    SceneData(
        const Device *device, const Allocator *allocator,
        const RenderPass *renderPass, const RaytraceData *raytraceData,
        const scene::ViewportScene &scene,
        vulkan::ShadingPipeline pipeline = vulkan::ShadingPipeline::RASTER,
        vulkan::ShadingType type = vulkan::ShadingType::BASIC);
    ~SceneData();

    SceneData(const SceneData &sceneData) = delete;

    const bool &isInitialized = m_isInitialized;

    void updateRaytracedFrameCount(bool reset = false);

    inline void setShadingPipeline(vulkan::ShadingPipeline pipeline)
    {
        m_currentShadingPipeline = pipeline;
    };
    inline vulkan::ShadingPipeline getCurrentShadingPipeline() const
    {
        return m_currentShadingPipeline;
    }
    inline void setShadingType(vulkan::ShadingType type)
    {
        m_currentShadingType = type;
    };
    inline vulkan::ShadingType getCurrentShadingType() const
    {
        return m_currentShadingType;
    }

    [[nodiscard]] inline const std::vector<MeshData> &getMeshData() const
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

    [[nodiscard]] inline const AllocatedBuffer &getRaytracedObjectBuffer() const
    {
        return m_raytracedObjectBuffer;
    }
};
} // namespace kirana::viewport::vulkan
#endif