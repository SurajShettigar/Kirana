#ifndef SCENE_DATA_HPP
#define SCENE_DATA_HPP

#include <vector>
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
    std::vector<std::unique_ptr<Shader>> m_shaders;
    std::vector<MaterialData> m_materials;
    std::vector<MeshData> m_meshes;
    mutable CameraData m_cameraData;

    const Device *const m_device;
    const Allocator *const m_allocator;
    const RenderPass *const m_renderPass;
    const DescriptorSetLayout *const m_globalDescSetLayout;
    const std::array<int, 2> m_windowResolution;

    const scene::Scene &m_scene;

    void setVertexDescription();
    const Shader *createShader(const std::string &shaderName);
    void createMaterials();

  public:
    SceneData(const Device *device, const Allocator *allocator,
              const RenderPass *renderPass,
              const DescriptorSetLayout *globalDescSetLayout,
              std::array<int, 2> windowResolution, const scene::Scene &scene);
    ~SceneData();

    SceneData(const SceneData &sceneData) = delete;

    const bool &isInitialized = m_isInitialized;
    const std::vector<MeshData> &meshes = m_meshes;

    [[nodiscard]] const CameraData &getCameraData() const;
};
} // namespace kirana::viewport::vulkan
#endif