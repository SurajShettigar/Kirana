#ifndef KIRANA_SCENE_SCENE_HPP
#define KIRANA_SCENE_SCENE_HPP

#include <utility>

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
    Scene(std::string name, std::vector<Mesh> meshes,
          std::vector<Material> materials, std::vector<Light> lights,
          std::vector<Camera> cameras)
        : Object{std::move(name)}, m_meshes{std::move(meshes)},
          m_materials{std::move(materials)}, m_lights{std::move(lights)},
          m_cameras{std::move(cameras)}
    {
        postProcess();
    }
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
    [[nodiscard]] inline const std::vector<math::Transform> &getTransforms()
        const
    {
        return m_globalTransforms;
    }
    [[nodiscard]] inline const std::vector<Node> &getNodes() const
    {
        return m_nodes;
    }

    const Object *getObjectAtNode(uint32_t nodeIndex) const
    {
        switch (m_nodes.at(nodeIndex).objectData.type)
        {
        case NodeObjectType::EMPTY:
            return &m_emptyObjects.at(m_nodeEmptyIndexTable.at(nodeIndex));
        case NodeObjectType::MESH:
            return &m_meshes.at(m_nodeMeshIndexTable.at(nodeIndex));
        case NodeObjectType::LIGHT:
            return &m_lights.at(m_nodeLightIndexTable.at(nodeIndex));
        case NodeObjectType::CAMERA:
            return &m_cameras.at(m_nodeCameraIndexTable.at(nodeIndex));
        }
        return nullptr;
    }

    [[nodiscard]] inline const Material &getMaterialForMesh(
        uint32_t meshIndex) const
    {
        return m_materials.at(m_meshMaterialIndexTable.at(meshIndex));
    }

    [[nodiscard]] inline const SceneStatistics &getStats() const
    {
        return m_stats;
    }

    uint32_t addNode(int parent,
                     NodeObjectType objectType = NodeObjectType::EMPTY,
                     int objectIndex = -1,
                     const math::Transform &transform = math::Transform{},
                     const std::string &name = "");

    inline void setMeshMaterial(uint32_t meshIndex, uint32_t materialIndex)
    {
        m_meshMaterialIndexTable[meshIndex] = materialIndex;
    }
    inline void setNodeRenderData(uint32_t nodeIndex, NodeRenderData renderData)
    {
        m_nodes.at(nodeIndex).renderData = renderData;
    }
    inline void setNodeObjectData(uint32_t nodeIndex, NodeObjectData objectData)
    {
        m_nodes.at(nodeIndex).objectData = std::move(objectData);
    }

    void postProcess();

  private:
    std::vector<Mesh> m_meshes;
    std::vector<Material> m_materials;
    std::vector<Light> m_lights;
    std::vector<Camera> m_cameras;

    std::vector<math::Transform> m_localTransforms;
    std::vector<math::Transform> m_globalTransforms;
    std::vector<Object> m_emptyObjects;
    std::vector<Node> m_nodes;

    std::unordered_map<uint32_t, uint32_t> m_nodeEmptyIndexTable;
    std::unordered_map<uint32_t, uint32_t> m_nodeMeshIndexTable;
    std::unordered_map<uint32_t, uint32_t> m_nodeLightIndexTable;
    std::unordered_map<uint32_t, uint32_t> m_nodeCameraIndexTable;
    std::unordered_map<uint32_t, uint32_t> m_nodeTransformIndexTable;

    std::unordered_map<uint32_t, uint32_t> m_meshMaterialIndexTable;

    SceneStatistics m_stats;
};
} // namespace kirana::scene
#endif