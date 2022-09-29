#ifndef SCENE_HPP
#define SCENE_HPP

#include <memory>
#include <vector>
#include <string>
#include "object.hpp"
#include "mesh.hpp"
#include "scene_utils.hpp"

struct aiScene;
struct aiNode;

namespace kirana::scene
{
class SceneImporter;
class Scene
{
    friend class SceneImporter;

  private:
    std::string m_name = "Scene";
    std::vector<std::shared_ptr<Mesh>> m_meshes;
    std::shared_ptr<Object> m_rootObject = nullptr;
    std::vector<std::shared_ptr<Object>> m_childObjects;

    std::shared_ptr<Object> &findObject(const aiNode *node);
    void getMeshesFromNode(const aiNode *node,
                           std::vector<std::shared_ptr<Mesh>> *nodeMeshes);
    void initializeChildObjects(const std::shared_ptr<Object> &parent,
                                uint32_t childCount, aiNode **children);
    void initFromAiScene(const aiScene *scene);

  public:
    Scene() = default;
    ~Scene() = default;

    Scene(const Scene &scene) = delete;

    const std::string &name = m_name;
    const std::shared_ptr<Object> &rootObject = m_rootObject;
    const std::vector<std::shared_ptr<Mesh>> &meshes = m_meshes;
};
} // namespace kirana::scene
#endif