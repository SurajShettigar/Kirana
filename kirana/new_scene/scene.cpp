#include "scene.hpp"

uint32_t kirana::scene::Scene::addNode(int parent, NodeObjectType objectType,
                                       int objectIndex,
                                       const math::Transform &transform,
                                       const std::string &name)
{
    const int newNodeIndex = static_cast<int>(m_nodes.size());

    int transformIndex = -1;
    // Only empty-nodes have transforms, other objects like mesh have
    // an empty-node as parent, so they have transforms through them.
    if (objectType == NodeObjectType::EMPTY)
    {
        transformIndex = static_cast<int>(m_localTransforms.size());
        m_localTransforms.push_back(transform);
        // TODO: Add transform multiplication operator
        if (parent > -1)
            m_globalTransforms.emplace_back(
                m_globalTransforms
                    .at(m_nodes.at(parent).objectData.transformIndex)
                    .getMatrix() *
                m_localTransforms.at(transformIndex).getMatrix());
        else
            m_globalTransforms.push_back(transform);

        m_nodeTransformIndexTable[newNodeIndex] = transformIndex;

        // Add empty object
        objectIndex = static_cast<int>(m_emptyObjects.size());
        std::string emptyName = name;
        if (emptyName.empty())
            emptyName = "Empty_" + std::to_string(objectIndex);
        m_emptyObjects.emplace_back(std::move(Object(emptyName)));
    }

    m_nodes.push_back(
        Node{parent, -1, -1, 0,
             NodeObjectData{objectType, objectIndex, transformIndex},
             NodeRenderData{}});
    Node &newNode = m_nodes.back();
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
        newNode.level = parentNode.level + 1;
    }
    if (objectIndex > -1)
    {
        switch (objectType)
        {
        case NodeObjectType::EMPTY:
            m_nodeEmptyIndexTable[newNodeIndex] = objectIndex;
            break;
        case NodeObjectType::MESH:
            m_nodeMeshIndexTable[newNodeIndex] = objectIndex;
            break;
        case NodeObjectType::LIGHT:
            m_nodeLightIndexTable[newNodeIndex] = objectIndex;
            break;
        case NodeObjectType::CAMERA:
            m_nodeCameraIndexTable[newNodeIndex] = objectIndex;
            break;
        }
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
    for (const auto &m : m_meshes)
    {
        m_stats.numVertices += static_cast<uint32_t>(m.getVertices().size());
        m_stats.numIndices += static_cast<uint32_t>(m.getIndices().size());
    }

    if (m_nodes.empty())
        return;

    // TODO: Do a topological-sort of nodes.
    //    for (const auto &n : m_nodes)
    //    {
    //        if (n.objectData.transformIndex < 0)
    //            continue;
    //        // TODO: Add transform multiplication operator
    //        if (n.parent > -1)
    //            m_globalTransforms
    //                .at(n.objectData.transformIndex) = math::Transform(
    //                m_globalTransforms
    //                    .at(m_nodes.at(n.parent).objectData.transformIndex)
    //                    .getMatrix() *
    //                m_localTransforms.at(n.objectData.transformIndex).getMatrix());
    //    }
}