#include "scene.hpp"


void kirana::scene::Scene::updateDirtyTransforms()
{
    // TODO: Ensure nodes are topologically sorted.
    if (m_dirtyTransformLevel == std::numeric_limits<uint32_t>::max())
        return;
    for (const auto &n : m_nodes)
    {
        if (n.level <= m_dirtyTransformLevel)
            continue;
        m_globalTransforms.at(n.objectData.transformIndex) = math::Transform(
            m_globalTransforms
                .at(m_nodes.at(n.parent).objectData.transformIndex)
                .getMatrix() *
            m_localTransforms.at(n.objectData.transformIndex).getMatrix());
    }
    m_dirtyTransformLevel = std::numeric_limits<uint32_t>::max();
}

void kirana::scene::Scene::setTransform(uint32_t nodeIndex,
                                        const math::Transform &transform,
                                        bool global)
{
    // Update the given transform
    const int transformIndex = getTransformIndexFromNode(nodeIndex);
    auto &t = global ? m_globalTransforms.at(transformIndex)
                     : m_localTransforms.at(transformIndex);
    t = transform;

    // Update the child transforms
    if (getNode(nodeIndex).level < m_dirtyTransformLevel)
        m_dirtyTransformLevel = getNode(nodeIndex).level;
    updateDirtyTransforms();

    // If camera, change global transform (to update view matrix).
    if (getNode(nodeIndex).objectData.type == NodeObjectType::CAMERA)
        m_cameras.at(getObjectIndexFromNode(nodeIndex))
            .setTransform(m_globalTransforms.at(transformIndex));
}


uint32_t kirana::scene::Scene::addNode(int parent, NodeObjectType objectType,
                                       int objectIndex,
                                       const math::Transform &transform,
                                       const std::string &emptyObjectName)
{
    const int newNodeIndex = static_cast<int>(m_nodes.size());

    // Add transform
    const int transformIndex = static_cast<int>(m_localTransforms.size());
    m_localTransforms.push_back(transform);
    // TODO: Add transform multiplication operator
    if (parent > -1)
        m_globalTransforms.emplace_back(
            m_globalTransforms.at(m_nodes.at(parent).objectData.transformIndex)
                .getMatrix() *
            m_localTransforms.at(transformIndex).getMatrix());
    else
        m_globalTransforms.push_back(transform);

    // Create an empty object.
    if (objectType == NodeObjectType::EMPTY)
    {
        objectIndex = static_cast<int>(m_emptyObjects.size());
        std::string emptyName = emptyObjectName;
        if (emptyName.empty())
            emptyName = "Empty_" + std::to_string(objectIndex);
        m_emptyObjects.emplace_back(std::move(Object(emptyName)));
    }

    m_nodes.push_back(
        Node{parent, -1, -1, 0,
             NodeObjectData{objectType, objectIndex, transformIndex},
             NodeRenderData{}});
    if (parent > -1)
    {
        Node &parentNode = m_nodes.at(parent);
        if (parentNode.child < 0)
            parentNode.child = newNodeIndex;
        else
        {
            Node &lastSiblingNode = m_nodes.at(parentNode.child);
            while (lastSiblingNode.sibling > -1)
                lastSiblingNode = m_nodes.at(lastSiblingNode.sibling);
            lastSiblingNode.sibling = newNodeIndex;
        }
        m_nodes[newNodeIndex].level = parentNode.level + 1;
    }
    return newNodeIndex;
}


void kirana::scene::Scene::postProcess()
{
    m_stats.numMeshes = static_cast<uint32_t>(m_meshes.size());
    m_stats.numMaterials = static_cast<uint32_t>(m_materials.size());
    m_stats.numLights = static_cast<uint32_t>(m_lights.size());
    m_stats.numCameras = static_cast<uint32_t>(m_cameras.size());

    m_stats.vertexSize = static_cast<uint32_t>(sizeof(scene::Vertex));
    m_stats.indexSize = static_cast<uint32_t>(sizeof(scene::INDEX_TYPE));
    m_stats.numVertices = 0;
    m_stats.numIndices = 0;
    // Create an object-index hash table
    for (uint32_t i = 0; i < m_meshes.size(); i++)
    {
        m_objectNameIndexTable[m_meshes[i].getName()] = i;
        m_stats.numVertices +=
            static_cast<uint32_t>(m_meshes[i].getVertices().size());
        m_stats.numIndices +=
            static_cast<uint32_t>(m_meshes[i].getIndices().size());
    }
    for (uint32_t i = 0; i < m_materials.size(); i++)
        m_objectNameIndexTable[m_materials[i].getName()] = i;
    for (uint32_t i = 0; i < m_lights.size(); i++)
        m_objectNameIndexTable[m_lights[i].getName()] = i;
    for (uint32_t i = 0; i < m_cameras.size(); i++)
        m_objectNameIndexTable[m_cameras[i].getName()] = i;

    if (m_nodes.empty())
        return;

    // TODO: Do a topological-sort of nodes.
}