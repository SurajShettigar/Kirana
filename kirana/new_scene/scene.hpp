#ifndef KIRANA_SCENE_SCENE_HPP
#define KIRANA_SCENE_SCENE_HPP

#include "mesh.hpp"
#include "material.hpp"
#include "light.hpp"
#include "camera.hpp"

#include <set>

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

    // Node functions
    [[nodiscard]] inline const std::vector<Node> &getNodes() const
    {
        return m_nodes;
    }
    [[nodiscard]] inline const Node &getNodeByName(
        const std::string &objectName) const
    {
        return m_nodes.at(
            m_objectNodeIndexTable.at(m_objectNameIndexTable.at(objectName)));
    }
    [[nodiscard]] const Object &getObjectAtNode(const Node &node) const
    {
        const int objectIndex = node.objectData.objectIndex;
        switch (node.objectData.type)
        {
        case NodeObjectType::MESH:
            return m_meshes.at(objectIndex);
        case NodeObjectType::LIGHT:
            return m_lights.at(objectIndex);
        case NodeObjectType::CAMERA:
            return m_cameras.at(objectIndex);
        case NodeObjectType::EMPTY:
        default:
            return m_emptyObjects.at(objectIndex);
        }
    }
    inline std::vector<Node> getSelectedNodes() const
    {
        std::vector<Node> nodes;
        for (const auto s : m_selectedNodes)
            nodes.push_back(m_nodes.at(s));
        return nodes;
    }

    // Mesh functions
    [[nodiscard]] inline const std::vector<Mesh> &getMeshes() const
    {
        return m_meshes;
    }
    inline const Mesh &getMeshByName(const std::string &meshName) const
    {
        return m_meshes.at(m_objectNameIndexTable.at(meshName));
    }
    inline const Mesh &getMeshAtNode(const Node &node) const
    {
        return dynamic_cast<const Mesh &>(getObjectAtNode(node));
    }
    // TODO: Remove index based setting of material.
    inline void setMeshMaterial(uint32_t meshIndex, uint32_t materialIndex)
    {
        m_meshMaterialIndexTable[meshIndex] = materialIndex;
    }
    inline void setMeshMaterial(const std::string &meshName,
                                const std::string &materialName)
    {
        setMeshMaterial(m_objectNameIndexTable.at(meshName),
                        m_objectNameIndexTable.at(materialName));
    }

    // Material functions
    [[nodiscard]] inline const std::vector<Material> &getMaterials() const
    {
        return m_materials;
    }
    inline const Material &getMaterialByName(const std::string &matName) const
    {
        return m_materials.at(m_objectNameIndexTable.at(matName));
    }
    inline Material &getMaterialByName(const std::string &matName)
    {
        return const_cast<Material &>(
            static_cast<const Scene &>(*this).getMaterialByName(matName));
    }
    inline const Material &getMaterialOfMesh(const Mesh &mesh) const
    {
        return m_materials.at(m_meshMaterialIndexTable.at(
            m_objectNameIndexTable.at(mesh.getName())));
    }
    inline Material &getMaterialOfMesh(const Mesh &mesh)
    {
        return const_cast<Material &>(
            static_cast<const Scene &>(*this).getMaterialOfMesh(mesh));
    }

    // Light functions
    [[nodiscard]] inline const std::vector<Light> &getLights() const
    {
        return m_lights;
    }
    inline const Light &getLightByName(const std::string &lightName) const
    {
        return m_lights.at(m_objectNameIndexTable.at(lightName));
    }
    inline const Light &getLightAtNode(const Node &node) const
    {
        return dynamic_cast<const Light &>(getObjectAtNode(node));
    }

    // Camera functions
    [[nodiscard]] inline const std::vector<Camera> &getCameras() const
    {
        return m_cameras;
    }
    inline const Camera &getCameraByName(const std::string &camName) const
    {
        return m_cameras.at(m_objectNameIndexTable.at(camName));
    }
    inline const Camera &getCameraAtNode(const Node &node) const
    {
        return dynamic_cast<const Camera &>(getObjectAtNode(node));
    }

    // Transform functions
    [[nodiscard]] inline const std::vector<math::Transform>
        &getGlobalTransforms() const
    {
        return m_globalTransforms;
    }
    [[nodiscard]] inline const math::Transform &getGlobalTransform(
        const Node &node) const
    {
        return m_globalTransforms.at(node.objectData.transformIndex);
    }
    [[nodiscard]] inline const math::Transform &getGlobalTransform(
        const std::string &objectName) const
    {
        return m_globalTransforms.at(
            getNodeByName(objectName).objectData.transformIndex);
    }
    [[nodiscard]] inline const math::Transform &getLocalTransform(
        const Node &node) const
    {
        return m_localTransforms.at(node.objectData.transformIndex);
    }
    [[nodiscard]] inline const math::Transform &getLocalTransform(
        const std::string &objectName) const
    {
        return m_localTransforms.at(
            getNodeByName(objectName).objectData.transformIndex);
    }

    inline void setGlobalTransform(const Node &node,
                                   const math::Transform &transform)
    {
        setTransform(node, transform, true);
    }
    inline void setGlobalTransform(const std::string &objectName,
                                   const math::Transform &transform)
    {
        setTransform(getNodeByName(objectName), transform, true);
    }
    inline void setLocalTransform(const Node &node,
                                  const math::Transform &transform)
    {
        setTransform(node, transform, false);
    }
    inline void setLocalTransform(const std::string &objectName,
                                  const math::Transform &transform)
    {
        setTransform(getNodeByName(objectName), transform, false);
    }

    [[nodiscard]] inline const SceneStatistics &getStats() const
    {
        return m_stats;
    }

    const Node &addNode(int parent,
                        NodeObjectType objectType = NodeObjectType::EMPTY,
                        int objectIndex = -1,
                        const math::Transform &transform = math::Transform{},
                        const std::string &emptyObjectName = "");
    void setNodeRenderData(const Node &node, NodeRenderData renderData);
    inline void setNodeObjectData(const Node &node, NodeObjectData objectData)
    {
        m_nodes.at(node.index).objectData = objectData;
    }

    void postProcess();

  private:
    std::vector<Mesh> m_meshes;
    std::vector<Material> m_materials;
    std::vector<Light> m_lights;
    std::vector<Camera> m_cameras;
    std::vector<Object> m_emptyObjects;

    std::vector<math::Transform> m_localTransforms;
    std::vector<math::Transform> m_globalTransforms;
    std::vector<Node> m_nodes;

    // Search objects
    std::unordered_map<uint32_t, uint32_t> m_objectNodeIndexTable;
    std::unordered_map<std::string, uint32_t> m_objectNameIndexTable;
    std::unordered_map<uint32_t, uint32_t> m_meshMaterialIndexTable;
    uint32_t m_dirtyTransformLevel = std::numeric_limits<uint32_t>::max();
    std::set<uint32_t> m_selectedNodes;

    SceneStatistics m_stats;
    void updateDirtyTransforms();
    void setTransform(const Node &node, const math::Transform &transform,
                      bool global = true);
};
} // namespace kirana::scene
#endif