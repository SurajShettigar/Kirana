#ifndef SCENE_HPP
#define SCENE_HPP

#include "object.hpp"
#include "mesh.hpp"
#include "material.hpp"
#include "scene_utils.hpp"

#include <memory>
#include <vector>
#include <string>
#include <event.hpp>

struct aiScene;
struct aiNode;

namespace kirana::scene
{
class SceneImporter;
class SceneManager;
class Scene
{
    friend class SceneImporter;
    friend class SceneManager;

  private:
    utils::Event<> m_onCameraChange;
    utils::Event<> m_onWorldChange;

    bool m_isInitialized = false;
    std::string m_name = "Scene";
    std::vector<std::shared_ptr<Mesh>> m_meshes;
    std::shared_ptr<Object> m_rootObject = nullptr;
    std::vector<std::shared_ptr<Object>>
        m_objects; // Also contains m_rootObject.
    std::vector<std::shared_ptr<Material>> m_materials;
    WorldData m_worldData;

    PerspectiveCamera m_camera{{1280, 720}, 60.0f, 0.1f, 1000.0f, true, true};

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
    [[nodiscard]] inline const WorldData &getWorldData() const
    {
        return m_worldData;
    }
    [[nodiscard]] inline const PerspectiveCamera &getCamera() const
    {
        return m_camera;
    }

    inline uint32_t addOnCameraChangeEventListener(
        const std::function<void()> &callback)
    {
        return m_onCameraChange.addListener(callback);
    }
    inline void removeOnCameraChangeEventListener(uint32_t callbackID)
    {
        m_onCameraChange.removeListener(callbackID);
    }
    inline uint32_t addOnWorldChangeEventListener(
        const std::function<void()> &callback)
    {
        return m_onWorldChange.addListener(callback);
    }
    inline void removeOnWorldChangeEventListener(uint32_t callbackID)
    {
        m_onWorldChange.removeListener(callbackID);
    }

    [[nodiscard]] std::vector<math::Transform *> getTransformsForMesh(
        const Mesh *mesh) const;

    void updateCameraResolution(std::array<uint32_t, 2> resolution) const;
};
} // namespace kirana::scene
#endif