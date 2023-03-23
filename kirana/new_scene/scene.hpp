#ifndef KIRANA_SCENE_SCENE_HPP
#define KIRANA_SCENE_SCENE_HPP

#include "mesh.hpp"
#include "material.hpp"
#include "light.hpp"
#include "camera.hpp"

namespace kirana::scene
{

class Scene : public Object
{
    friend class external::AssimpSceneConverter;

  public:
    Scene() = default;
    explicit Scene(std::string name) : Object(std::move(name)){};
    ~Scene() override = default;

    [[nodiscard]] inline bool isEmpty() const
    {
        return m_nodes.empty();
    }

    [[nodiscard]] inline const std::vector<Mesh> &getMeshes() const
    {
        return m_meshes;
    }
    [[nodiscard]] inline const std::vector<Material> &getMaterials() const
    {
        return m_materials;
    }
    [[nodiscard]] inline const std::vector<Light> &getLights() const
    {
        return m_lights;
    }
    [[nodiscard]] inline const std::vector<Camera> &getCameras() const
    {
        return m_cameras;
    }

    [[nodiscard]] inline const std::vector<Node> &getNodes() const
    {
        return m_nodes;
    }

    [[nodiscard]] inline const Material &getMaterialForMesh(uint32_t meshIndex) const
    {
        return m_materials.at(m_meshMaterialIndexTable.at(meshIndex));
    }

    [[nodiscard]] inline const SceneStatistics &getStats() const
    {
        return m_stats;
    }

  private:
    std::vector<Mesh> m_meshes;
    std::vector<Material> m_materials;
    std::vector<Light> m_lights;
    std::vector<Camera> m_cameras;
    std::vector<math::Transform> m_localTransforms;
    std::vector<math::Transform> m_globalTransforms;

    std::vector<Node> m_nodes;

    std::unordered_map<uint32_t, uint32_t> m_nodeMeshIndexTable;
    std::unordered_map<uint32_t, uint32_t> m_nodeLightIndexTable;
    std::unordered_map<uint32_t, uint32_t> m_nodeCameraIndexTable;
    std::unordered_map<uint32_t, uint32_t> m_nodeTransformIndexTable;

    std::unordered_map<uint32_t, uint32_t> m_meshMaterialIndexTable;

    SceneStatistics m_stats;

    uint32_t addNode(int parent, ObjectType objectType = ObjectType::EMPTY,
                     int objectIndex = -1, int transformIndex = -1,
                     const std::string &name = "");

    inline void linkMaterialToMesh(uint32_t meshIndex, uint32_t materialIndex)
    {
        m_meshMaterialIndexTable[meshIndex] = materialIndex;
    }
};
} // namespace kirana::scene
#endif