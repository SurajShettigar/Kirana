#ifndef SCENE_DATA_HPP
#define SCENE_DATA_HPP

#include <vector>
#include <unordered_map>
#include "vulkan_types.hpp"

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
class SceneData
{
  private:
    bool m_isInitialized = false;
    uint16_t m_currentShading = 0;
    VertexInputDescription m_vertexDesc;
    std::unordered_map<std::string, std::unique_ptr<Shader>> m_shaders;
    mutable std::unordered_map<std::string, MaterialData> m_materials;
    std::unordered_map<std::string, MeshData> m_meshes;
    CameraData m_cameraData;
    AllocatedBuffer m_cameraBuffer;
    AllocatedBuffer m_worldDataBuffer;

    const Device *const m_device;
    const Allocator *const m_allocator;
    const RenderPass *m_renderPass;
    const DescriptorSetLayout *const m_globalDescSetLayout;

    const scene::ViewportScene &m_scene;
    uint32_t m_cameraChangeListener;
    uint32_t m_worldChangeListener;

    void setVertexDescription();

    void onWorldChanged();
    void onCameraChanged();

    void createWorldDataBuffer();
    void createCameraBuffer();

    const Shader *createShader(const std::string &shaderName);
    MaterialData getMaterialData(const scene::Material &material);
    void createMaterials();
    MaterialData &findMaterial(const std::string &materialName,
                               bool overrideShading = false);

    bool createMeshes();

  public:
    SceneData(const Device *device, const Allocator *allocator,
              const RenderPass *renderPass,
              const DescriptorSetLayout *globalDescSetLayout,
              const scene::ViewportScene &scene, uint16_t shadingIndex = 0);
    ~SceneData();

    SceneData(const SceneData &sceneData) = delete;

    const bool &isInitialized = m_isInitialized;

    [[nodiscard]] inline const std::unordered_map<std::string, MeshData>
        &getMeshData() const
    {
        return m_meshes;
    }

    inline bool shouldRenderOutline() const
    {
        return m_currentShading == 0;
    }
    [[nodiscard]] const MaterialData &getOutlineMaterial() const;

    void setShading(uint16_t shadingIndex);
    void rebuildPipeline(const RenderPass *renderPass);

    [[nodiscard]] const AllocatedBuffer &getCameraBuffer() const;
    [[nodiscard]] uint32_t getCameraBufferOffset(uint32_t offsetIndex) const;

    [[nodiscard]] const AllocatedBuffer &getWorldDataBuffer() const;
    [[nodiscard]] uint32_t getWorldDataBufferOffset(uint32_t offsetIndex) const;
};
} // namespace kirana::viewport::vulkan
#endif