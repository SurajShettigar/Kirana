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
    bool m_isInitialized = false;
    std::string m_name = "Scene";
    std::vector<std::shared_ptr<Mesh>> m_meshes;
    std::shared_ptr<Object> m_rootObject = nullptr;
    std::vector<std::shared_ptr<Object>>
        m_objects; // Also contains m_rootObject.

    PerspectiveCamera m_camera {{1280, 720}, 60.0f, 0.1f, 1000.0f, true, true};

    const Object *findObject(const aiNode *node) const;
    void getMeshesFromNode(const aiNode *node,
                           std::vector<std::shared_ptr<Mesh>> *nodeMeshes);
    void initializeChildObjects(std::shared_ptr<Object> parent,
                                uint32_t childCount, aiNode **children);
    void initFromAiScene(const aiScene *scene);

  public:
    Scene() = default;
    ~Scene() = default;

    Scene(const Scene &scene) = delete;

    [[nodiscard]] inline bool isInitialized() const
    {
        return m_isInitialized;
    }

    [[nodiscard]] inline const std::string &getName() const
    {
        return m_name;
    }
    [[nodiscard]] inline const std::shared_ptr<Object> &getRoot() const
    {
        return m_rootObject;
    }
    [[nodiscard]] inline const std::vector<std::shared_ptr<Object>>
        &getObjects() const
    {
        return m_objects;
    }
    [[nodiscard]] inline const std::vector<std::shared_ptr<Mesh>> &getMeshes()
        const
    {
        return m_meshes;
    }

    [[nodiscard]] inline const PerspectiveCamera &getCamera() const
    {
        return m_camera;
    }

    [[nodiscard]] std::vector<math::Transform *> getTransformsForMesh(
        const Mesh *mesh) const;
    [[nodiscard]] math::Matrix4x4 getClipSpaceMatrix(
        math::Transform *model) const;
};
} // namespace kirana::scene
#endif