#ifndef KIRANA_SCENE_SCENE_HPP
#define KIRANA_SCENE_SCENE_HPP

#include "scene_graph.hpp"
#include "mesh.hpp"
#include "material.hpp"
#include "light.hpp"
#include "camera.hpp"

namespace kirana::scene
{

class Scene : public Object
{
    friend class converters::AssimpConverter;
  public:
    Scene() = default;
    explicit Scene(std::string name) : Object(std::move(name)){};
    ~Scene() = default;

  private:
    std::vector<Mesh> m_meshes;
    std::vector<Material> m_materials;
    std::vector<Light> m_lights;
    std::vector<Camera> m_cameras;
    std::vector<math::Transform> m_localTransforms;
    std::vector<math::Transform> m_globalTransforms;

    std::unordered_map<uint32_t, uint32_t> m_nodeMeshIndexTable;
    std::unordered_map<uint32_t, uint32_t> m_meshMaterialIndexTable;
    std::unordered_map<uint32_t, uint32_t> m_nodeLightIndexTable;
    std::unordered_map<uint32_t, uint32_t> m_nodeCameraIndexTable;
    std::unordered_map<uint32_t, uint32_t> m_nodeTransformIndexTable;

    SceneGraph m_sceneGraph;
};
} // namespace kirana::scene
#endif