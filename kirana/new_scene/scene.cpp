#include "scene.hpp"

uint32_t kirana::scene::Scene::addNode(int parent, ObjectType objectType,
                                       int objectIndex, int transformIndex)
{
    const int newNodeIndex = static_cast<int>(m_nodes.size());

    m_nodes.push_back(
        Node{parent, -1, -1, 0, objectType, objectIndex, transformIndex});
    Node &newNode = m_nodes.back();
    if (parent > -1)
    {
        Node &parentNode = m_nodes.at(parent);
        if (parentNode.child == -1)
            parentNode.child = newNodeIndex;
        else
        {
            Node &lastSiblingNode = m_nodes.at(parentNode.child);
            while (lastSiblingNode.sibling != -1)
                lastSiblingNode = m_nodes.at(lastSiblingNode.sibling);
            lastSiblingNode.sibling = newNodeIndex;
        }
        newNode.level = parentNode.level + 1;
    }

    if (objectIndex > -1)
    {
        switch (objectType)
        {
        case ObjectType::EMPTY:
            break;
        case ObjectType::MESH:
            m_nodeMeshIndexTable[newNodeIndex] = objectIndex;
            break;
        case ObjectType::LIGHT:
            m_nodeLightIndexTable[newNodeIndex] = objectIndex;
            break;
        case ObjectType::CAMERA:
            m_nodeCameraIndexTable[newNodeIndex] = objectIndex;
            break;
        }
    }

    if (transformIndex > -1)
        m_nodeTransformIndexTable[newNodeIndex] = transformIndex;

    return static_cast<uint32_t>(newNodeIndex);
}