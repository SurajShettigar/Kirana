#include <iostream>

#include "../scene_manager.hpp"

using namespace kirana::scene;

std::string getType(ObjectType objectType)
{
    switch (objectType)
    {
    case ObjectType::EMPTY:
        return "EMPTY";
    case ObjectType::MESH:
        return "MESH";
    case ObjectType::LIGHT:
        return "LIGHT";
    case ObjectType::CAMERA:
        return "CAMERA";
    }
    return "";
}

int main(int argc, char **argv)
{
    SceneManager &sceneMgr = SceneManager::get();
    if (sceneMgr.loadDefaultScene())
    {
        const Scene &scene = sceneMgr.getCurrentScene();
        std::cout << "Loaded Scene: " << scene.getName() << std::endl;
        std::cout << "Stats: " << std::string(scene.getStats()) << std::endl;
        const auto &nodes = scene.getNodes();
        std::cout << "Node Hierarchy:" << std::endl;
        for (const auto & node : nodes)
        {
            std::string tabs;
            for (uint32_t l = 0; l < node.level; l++)
                tabs += "\t";
            const std::string matName =
                node.objectType == ObjectType::MESH &&
                        node.objectIndex > -1
                    ? " (Material: " +
                          scene.getMaterialForMesh(node.objectIndex)
                              .getName() +
                          ") "
                    : "";

            std::cout << tabs << node.name /*<< " ["
                      << getType(node.objectType) << "] "*/ << matName
                      << std::endl;
        }
    }
    else
        std::cerr << "Failed to load default scene" << std::endl;

    return 0;
}