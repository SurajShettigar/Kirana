#ifndef SCENE_DATA_HPP
#define SCENE_DATA_HPP

#include <vector>
#include "vulkan_types.hpp"

namespace kirana::scene
{
class Scene;
}

namespace kirana::viewport::vulkan
{
class Allocator;
class SceneData
{
  private:
    bool m_isInitialized = false;
    VertexInputDescription m_vertexDesc;
    std::vector<MeshData> m_meshes;

    const Allocator *const m_allocator;

    void setVertexDescription();

  public:
    SceneData(const Allocator *allocator, const scene::Scene &scene);
    ~SceneData();

    SceneData(const SceneData &sceneData) = delete;

    const bool &isInitialized = m_isInitialized;
    const VertexInputDescription &vertexDesc = m_vertexDesc;
    const std::vector<MeshData> &meshes = m_meshes;
};
} // namespace kirana::viewport::vulkan
#endif