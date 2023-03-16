#ifndef KIRANA_SCENE_SCENE_GRAPH_HPP
#define KIRANA_SCENE_SCENE_GRAPH_HPP

#include "scene_types.hpp"

namespace kirana::scene
{
class SceneGraph
{
  public:
    SceneGraph() = default;
    ~SceneGraph() = default;

    uint32_t addNode(int parent)
    {
        const int newNodeIndex = static_cast<int>(m_nodes.size());

        m_nodes.push_back(Node{parent, -1, -1, 0});
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
        return static_cast<uint32_t>(newNodeIndex);
    }

  private:
    std::vector<Node> m_nodes;
};

} // namespace kirana::scene

#endif