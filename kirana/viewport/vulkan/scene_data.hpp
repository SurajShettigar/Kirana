#ifndef SCENE_DATA_HPP
#define SCENE_DATA_HPP

#include <vector>
#include <unordered_map>
#include "vulkan_types.hpp"

namespace kirana::scene
{
class Scene;
class Material;
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
    VertexInputDescription m_vertexDesc;
    std::unordered_map<std::string, std::unique_ptr<Shader>> m_shaders;
    std::vector<MaterialData> m_materials;
    std::vector<MeshData> m_meshes;
    mutable CameraData m_cameraData;
    AllocatedBuffer m_cameraBuffer;
    AllocatedBuffer m_worldDataBuffer;

    const Device *const m_device;
    const Allocator *const m_allocator;
    const RenderPass *m_renderPass;
    const DescriptorSetLayout *const m_globalDescSetLayout;

    const scene::Scene &m_scene;

    void setVertexDescription();
    const Shader *createShader(const std::string &shaderName);
    void createMaterials();
    void createCameraBuffer();
    void createWorldDataBuffer();
  public:
    SceneData(const Device *device, const Allocator *allocator,
              const RenderPass *renderPass,
              const DescriptorSetLayout *globalDescSetLayout, const scene::Scene &scene);
    ~SceneData();

    SceneData(const SceneData &sceneData) = delete;

    const bool &isInitialized = m_isInitialized;
    const std::vector<MeshData> &meshes = m_meshes;

    void rebuildPipeline(const RenderPass *renderPass);

    [[nodiscard]] const AllocatedBuffer &getCameraBuffer() const;
    [[nodiscard]] uint32_t getCameraBufferOffset(uint32_t offsetIndex) const;

    [[nodiscard]] const AllocatedBuffer &getWorldDataBuffer() const;
    [[nodiscard]] uint32_t getWorldDataBufferOffset(uint32_t offsetIndex) const;
    void updateWorldDataBuffer(uint32_t offsetIndex) const;
};
} // namespace kirana::viewport::vulkan
#endif