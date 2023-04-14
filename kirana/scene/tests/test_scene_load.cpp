#include <iostream>

#include "../scene_manager.hpp"

using namespace kirana::scene;

std::string getType(NodeObjectType objectType)
{
    switch (objectType)
    {
    case NodeObjectType::EMPTY:
        return "EMPTY";
    case NodeObjectType::MESH:
        return "MESH";
    case NodeObjectType::LIGHT:
        return "LIGHT";
    case NodeObjectType::CAMERA:
        return "CAMERA";
    }
    return "";
}

void showSceneInfo(const Scene &scene)
{
    std::cout << "Loaded Scene: " << scene.getName() << std::endl;
    std::cout << "Stats: " << std::string(scene.getStats()) << std::endl;
    const auto &nodes = scene.getNodes();
    std::cout << "Node Hierarchy:" << std::endl;
    for (const auto &node: nodes)
    {
        std::string tabs;
        for (uint32_t l = 0; l < node.level; l++)
            tabs += "\t";
        const std::string matName =
            node.objectData.type == NodeObjectType::MESH &&
                    node.objectData.objectIndex > -1
                ? " (Material: " +
                      scene.getMaterialOfMesh(scene.getMeshAtNode(node))
                          .getName() +
                      ") "
                : "";

        const std::string nodeName = scene.getObjectAtNode(node).getName();
        std::cout << tabs << nodeName /*<< " ["
                  << getType(node.objectType) << "] "*/
                  << matName << std::endl;
    }
}

int main(int argc, char **argv)
{
    SceneManager &sceneMgr = SceneManager::get();
    sceneMgr.init();
    showSceneInfo(sceneMgr.getEditorScene());
    if (sceneMgr.loadDefaultScene())
        showSceneInfo(sceneMgr.getCurrentScene());
    else
        std::cerr << "Failed to load default scene" << std::endl;

    return 0;
}