#ifndef SCENE_HPP
#define SCENE_HPP

#include "object.hpp"
#include "mesh.hpp"
#include "material.hpp"
#include "perspective_camera.hpp"
#include "scene_utils.hpp"

#include <memory>
#include <vector>
#include <string>
#include <event.hpp>

struct aiScene;
struct aiNode;

namespace kirana::scene
{
namespace primitives
{
class Plane;
}
class SceneImporter;
class SceneManager;
class Scene
{
    friend class SceneImporter;
    friend class SceneManager;

  private:
    utils::Event<> m_onWorldChange;
    utils::Event<Object> m_onActiveSelectionChange;

    bool m_isInitialized = false;

    // Scene data
    std::string m_name = "Scene";
    std::vector<std::shared_ptr<Mesh>> m_meshes;
    std::shared_ptr<Object> m_rootObject = nullptr;
    std::vector<std::shared_ptr<Object>>
        m_objects; // Also contains m_rootObject.
    std::vector<std::shared_ptr<Material>> m_materials;
    std::vector<Camera> m_cameras;
    WorldData m_worldData;

    // Active scene properties
    std::unique_ptr<Camera> m_viewportCamera;
    std::unique_ptr<primitives::Plane> m_grid;
    std::vector<const Object *> m_activeSelection;

    void getMeshesFromNode(const aiNode *node,
                           std::vector<std::shared_ptr<Mesh>> *nodeMeshes);
    void initializeChildObjects(std::shared_ptr<Object> parent,
                                uint32_t childCount, aiNode **children);
    void initFromAiScene(const aiScene *scene);

  public:
    Scene();
    ~Scene();

    Scene(const Scene &scene) = delete;

    inline uint32_t addOnWorldChangeEventListener(
        const std::function<void()> &callback)
    {
        return m_onWorldChange.addListener(callback);
    }
    inline void removeOnWorldChangeEventListener(uint32_t callbackID)
    {
        m_onWorldChange.removeListener(callbackID);
    }

    [[nodiscard]] inline bool isInitialized() const
    {
        return m_isInitialized;
    }

    [[nodiscard]] inline const std::string &getName() const
    {
        return m_name;
    }

    [[nodiscard]] inline const std::vector<std::shared_ptr<Mesh>> &getMeshes()
        const
    {
        return m_meshes;
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
    [[nodiscard]] inline const std::vector<std::shared_ptr<Material>>
        &getMaterials() const
    {
        return m_materials;
    }
    [[nodiscard]] inline WorldData &getWorldData()
    {
        return m_worldData;
    }
    [[nodiscard]] inline Camera *getActiveCamera()
    {
        // TODO: Replace with current active camera.
        return m_viewportCamera.get();
    }

    [[nodiscard]] std::vector<const kirana::scene::Object *>
    getViewportObjects();

    [[nodiscard]] std::vector<math::Transform *> getTransformsForMesh(
        const Mesh *mesh) const;

    [[nodiscard]] inline std::vector<const Object *> getActiveSelection() const
    {
        return m_activeSelection;
    }
};
} // namespace kirana::scene
#endif