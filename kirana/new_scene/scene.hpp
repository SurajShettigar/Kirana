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
    [[nodiscard]] inline const Node &getNode(uint32_t index) const
    {
        return m_nodes.at(index);
    }
    [[nodiscard]] inline int getObjectIndexFromName(
        const std::string &name) const
    {
        return m_objectNameIndexTable.find(name) == m_objectNameIndexTable.end()
                   ? -1
                   : static_cast<int>(m_objectNameIndexTable.at(name));
    }
    [[nodiscard]] inline int getObjectIndexFromNode(uint32_t nodeIndex) const
    {
        return getNode(nodeIndex).objectData.objectIndex;
    }
    [[nodiscard]] inline int getTransformIndexFromNode(uint32_t nodeIndex) const
    {
        return getNode(nodeIndex).objectData.transformIndex;
    }

    // Mesh functions
    [[nodiscard]] inline const std::vector<Mesh> &getMeshes() const
    {
        return m_meshes;
    }
    [[nodiscard]] inline const Mesh &getMesh(const uint32_t meshIndex) const
    {
        return m_meshes.at(meshIndex);
    }
    inline void setMeshMaterial(uint32_t meshIndex, uint32_t materialIndex)
    {
        m_meshMaterialIndexTable[meshIndex] = materialIndex;
    }

    // Material functions
    [[nodiscard]] inline const std::vector<Material> &getMaterials() const
    {
        return m_materials;
    }
    [[nodiscard]] inline const Material &getMaterial(uint32_t index) const
    {
        return m_materials.at(index);
    }
    inline Material &getMaterial(uint32_t index)
    {
        return const_cast<Material &>(
            static_cast<const Scene &>(*this).getMaterial(index));
    }
    [[nodiscard]] inline const Material &getMaterialOfMesh(
        uint32_t meshIndex) const
    {
        return getMaterial(m_meshMaterialIndexTable.at(meshIndex));
    }
    inline Material &getMaterialOfMesh(uint32_t meshIndex)
    {
        return const_cast<Material &>(
            static_cast<const Scene &>(*this).getMaterialOfMesh(meshIndex));
    }

    // Light functions
    [[nodiscard]] inline const std::vector<Light> &getLights() const
    {
        return m_lights;
    }
    [[nodiscard]] inline const Light &getLight(uint32_t index) const
    {
        return m_lights.at(index);
    }
    inline Light &getLight(uint32_t index)
    {
        return const_cast<Light &>(
            static_cast<const Scene &>(*this).getLight(index));
    }

    // Camera functions
    [[nodiscard]] inline const std::vector<Camera> &getCameras() const
    {
        return m_cameras;
    }
    [[nodiscard]] inline const Camera &getCamera(uint32_t index) const
    {
        return m_cameras.at(index);
    }
    inline Camera &getCamera(uint32_t index)
    {
        return const_cast<Camera &>(
            static_cast<const Scene &>(*this).getCamera(index));
    }

    // Transform functions
    [[nodiscard]] inline const std::vector<math::Transform>
        &getGlobalTransforms() const
    {
        return m_globalTransforms;
    }
    [[nodiscard]] inline math::Transform getGlobalTransform(
        uint32_t nodeIndex) const
    {
        return m_globalTransforms.at(getTransformIndexFromNode(nodeIndex));
    }
    [[nodiscard]] inline math::Transform getLocalTransform(
        uint32_t nodeIndex) const
    {
        return m_localTransforms.at(getTransformIndexFromNode(nodeIndex));
    }
    inline void setGlobalTransform(uint32_t nodeIndex,
                                   const math::Transform &transform)
    {
        setTransform(nodeIndex, transform, true);
    }
    inline void setLocalTransform(uint32_t nodeIndex,
                                  const math::Transform &transform)
    {
        setTransform(nodeIndex, transform, false);
    }

    [[nodiscard]] const Object *getObjectAtNode(uint32_t nodeIndex) const
    {
        const int objectIndex = getObjectIndexFromNode(nodeIndex);
        switch (m_nodes.at(nodeIndex).objectData.type)
        {
        case NodeObjectType::EMPTY:
            return &m_emptyObjects.at(objectIndex);
        case NodeObjectType::MESH:
            return &m_meshes.at(objectIndex);
        case NodeObjectType::LIGHT:
            return &m_lights.at(objectIndex);
        case NodeObjectType::CAMERA:
            return &m_cameras.at(objectIndex);
        }
        return nullptr;
    }

    [[nodiscard]] inline const SceneStatistics &getStats() const
    {
        return m_stats;
    }

    uint32_t addNode(int parent,
                     NodeObjectType objectType = NodeObjectType::EMPTY,
                     int objectIndex = -1,
                     const math::Transform &transform = math::Transform{},
                     const std::string &emptyObjectName = "");
    inline void setNodeRenderData(uint32_t nodeIndex, NodeRenderData renderData)
    {
        m_nodes.at(nodeIndex).renderData = renderData;
    }
    inline void setNodeObjectData(uint32_t nodeIndex, NodeObjectData objectData)
    {
        m_nodes.at(nodeIndex).objectData = objectData;
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
    std::unordered_map<std::string, uint32_t> m_objectNameIndexTable;
    std::unordered_map<uint32_t, uint32_t> m_meshMaterialIndexTable;
    uint32_t m_dirtyTransformLevel = std::numeric_limits<uint32_t>::max();
    SceneStatistics m_stats;

    void updateDirtyTransforms();
    void setTransform(uint32_t nodeIndex,
                      const math::Transform &transform, bool global=true);
};
} // namespace kirana::scene
#endif