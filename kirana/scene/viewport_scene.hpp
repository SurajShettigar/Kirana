#ifndef VIEWPORT_SCENE_HPP
#define VIEWPORT_SCENE_HPP

#include "scene.hpp"

namespace kirana::scene
{
namespace primitives
{
class Plane;
}
class Object;
class SceneImporter;
class SceneManager;
class ViewportScene
{
  public:
    friend class SceneImporter;
    friend class SceneManager;

  private:
    mutable utils::Event<> m_onWorldChange;
    mutable utils::Event<const Scene &, bool> m_onSceneLoaded;

    WorldData m_worldData;
    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<primitives::Plane> m_grid;
    Scene m_currentScene;

    std::unordered_map<std::string, size_t> m_sceneObjectIndexTable;
    std::unordered_map<std::string, bool> m_sceneObjectSelectionTable;

    std::vector<std::shared_ptr<Material>> m_materials;
    std::vector<Renderable> m_renderables;

    void addDefaultMaterials();
    void addDefaultRenderables();
    void onSceneLoaded();
    void toggleObjectSelection(const std::string &objectName = "",
                               bool multiSelect = false);
    ViewportScene();
    ~ViewportScene();

  public:
    ViewportScene(const ViewportScene &viewportScene) = delete;

    static ViewportScene &get()
    {
        static ViewportScene instance;
        return instance;
    }

    inline uint32_t addOnWorldChangeEventListener(
        const std::function<void()> &callback) const
    {
        return m_onWorldChange.addListener(callback);
    }
    inline void removeOnWorldChangeEventListener(uint32_t callbackID) const
    {
        m_onWorldChange.removeListener(callbackID);
    }

    inline uint32_t addOnCameraChangeEventListener(
        const std::function<void()> &callback) const
    {
        return m_camera->addOnCameraChangeEventListener(callback);
    }
    inline void removeOnCameraChangeEventListener(uint32_t callbackID) const
    {
        m_camera->removeOnCameraChangeEventListener(callbackID);
    }

    inline uint32_t addOnSceneLoadedEventListener(
        const std::function<void(const Scene &, bool)> &callback) const
    {
        return m_onSceneLoaded.addListener(callback);
    }
    inline void removeOnSceneLoadedEventListener(uint32_t callbackID) const
    {
        m_onSceneLoaded.removeListener(callbackID);
    }

    [[nodiscard]] inline const WorldData &getWorldData() const
    {
        return m_worldData;
    }

    [[nodiscard]] inline const Camera &getCamera() const
    {
        return *m_camera;
    }

    [[nodiscard]] inline const Scene &getCurrentScene() const
    {
        return m_currentScene;
    }

    [[nodiscard]] inline const std::vector<std::shared_ptr<Material>>
        &getRenderableMaterials() const
    {
        return m_materials;
    }

    [[nodiscard]] inline const std::vector<Renderable> &getRenderables() const
    {
        return m_renderables;
    }

    [[nodiscard]] std::vector<Renderable> getSelectedRenderables() const
    {
        // TODO: Find a faster way to filter selected renderables.
        std::vector<Renderable> selectedRenderables;
        for (const auto &r : m_renderables)
            if (r.selected)
                selectedRenderables.push_back(r);
        return selectedRenderables;
    }

    void setCameraResolution(const std::array<uint32_t, 2> &resolution)
    {
        m_camera->setResolution(resolution);
    }
};
} // namespace kirana::scene

#endif