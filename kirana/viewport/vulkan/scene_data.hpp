#ifndef SCENE_DATA_HPP
#define SCENE_DATA_HPP

#include <vector>
#include "vulkan_types.hpp"

namespace kirana::scene
{
class Scene;
class Material;
}

namespace kirana::viewport::vulkan
{
class Shader;
class Device;
class RenderPass;
class Allocator;
class SceneData
{
  private:
    bool m_isInitialized = false;
    VertexInputDescription m_vertexDesc;
    std::vector<std::unique_ptr<Shader>> m_shaders;
    std::vector<MaterialData> m_materials;
    std::vector<MeshData> m_meshes;

    const Device *const m_device;
    const RenderPass *const m_renderPass;
    const Allocator *const m_allocator;
    const std::array<int, 2> m_windowResolution;

    const scene::Scene &m_scene;

    void setVertexDescription();
    const Shader *createShader(const std::string &shaderName);
    void createMaterials();
  public:
    SceneData(const Device *device, const RenderPass *renderPass,
              const Allocator *allocator,
              std::array<int, 2> windowResolution,
              const scene::Scene &scene);
    ~SceneData();

    SceneData(const SceneData &sceneData) = delete;

    const bool &isInitialized = m_isInitialized;
    const VertexInputDescription &vertexDesc = m_vertexDesc;
    const std::vector<MeshData> &meshes = m_meshes;

    [[nodiscard]] math::Matrix4x4 getClipSpaceMatrix(
        size_t meshIndex, size_t instanceIndex) const;
};
} // namespace kirana::viewport::vulkan
#endif