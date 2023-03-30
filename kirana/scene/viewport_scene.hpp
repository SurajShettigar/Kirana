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
    mutable utils::Event<bool> m_onSceneLoaded;

    WorldData m_worldData;
    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<primitives::Plane> m_grid;

    std::unordered_map<std::string, uint32_t> m_objectIndexTable;
    std::unordered_map<std::string, bool> m_objectSelectionTable;

    std::vector<std::shared_ptr<Material>> m_editorMaterials;
    std::vector<Renderable> m_editorRenderables;

    Scene m_currentScene;
    std::vector<Renderable> m_sceneRenderables;
    bool m_isSceneLoaded = false;

    SceneInfo m_editorSceneInfo;
    SceneInfo m_sceneInfo;

    void initializeEditorObjects();
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
        const std::function<void(bool)> &callback) const
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

    [[nodiscard]] inline CameraData getCameraData() const
    {
        return CameraData{
            m_camera->getViewMatrix(),
            m_camera->getProjectionMatrix(),
            m_camera->getViewProjectionMatrix(),
            math::Matrix4x4::inverse(m_camera->getViewProjectionMatrix()),
            m_camera->getTransform().getPosition(),
            m_camera->getTransform().getForward(),
            m_camera->nearPlane,
            m_camera->farPlane};
    }

    [[nodiscard]] inline const Scene &getCurrentScene() const
    {
        return m_currentScene;
    }

    [[nodiscard]] inline const std::vector<std::shared_ptr<Material>>
        &getEditorMaterials() const
    {
        return m_editorMaterials;
    }

    [[nodiscard]] inline const std::vector<Renderable> &getEditorRenderables()
        const
    {
        return m_editorRenderables;
    }

    [[nodiscard]] inline const std::vector<std::shared_ptr<Material>>
        &getSceneMaterials() const
    {
        return m_currentScene.getMaterials();
    }

    [[nodiscard]] inline const std::vector<Renderable> &getSceneRenderables()
        const
    {
        return m_sceneRenderables;
    }

    inline bool isSceneLoaded() const
    {
        return m_isSceneLoaded;
    }

    [[nodiscard]] std::vector<Renderable> getSelectedRenderables() const
    {
        // TODO: Find a faster way to filter selected renderables.
        std::vector<Renderable> selectedRenderables;
        for (const auto &r : m_sceneRenderables)
            if (r.selected)
                selectedRenderables.push_back(r);
        return selectedRenderables;
    }

    [[nodiscard]] inline const SceneInfo &getEditorSceneInfo() const
    {
        return m_editorSceneInfo;
    }

    [[nodiscard]] inline const SceneInfo &getSceneInfo() const
    {
        return m_sceneInfo;
    }

    void setCameraResolution(const std::array<uint32_t, 2> &resolution)
    {
        m_camera->setResolution(resolution);
    }
};
} // namespace kirana::scene

#endif