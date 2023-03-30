#ifndef KIRANA_SCENE_SCENE_MANAGER_HPP
#define KIRANA_SCENE_SCENE_MANAGER_HPP

#include "scene_types.hpp"
#include "scene.hpp"
#include "scene_editor.hpp"

namespace kirana::scene
{
class SceneManager
{
  public:
    SceneManager(const SceneManager &sceneManager) = delete;

    inline static SceneManager &get()
    {
        static SceneManager instance;
        return instance;
    }

    void init();
    void update();
    void clean();

    inline void setWindowResolution(const math::Vector2 &windowResolution)
    {
        m_sceneEditor.setWindowResolution(windowResolution);
    }

    bool loadScene(const std::string &scenePath,
                   const SceneImportSettings &settings = {});
    bool loadDefaultScene();

    [[nodiscard]] inline const Scene &getEditorScene() const
    {
        return m_editorScene;
    }
    [[nodiscard]] inline const Scene &getCurrentScene() const
    {
        return m_currentScene;
    }

  private:
    Scene m_editorScene;

    std::string m_currentScenePath;
    Scene m_currentScene;

    SceneEditor m_sceneEditor;

    SceneManager()
        : m_editorScene{}, m_currentScenePath{}, m_currentScene{},
          m_sceneEditor{m_editorScene, m_currentScene} {};
    ~SceneManager() = default;

    static void postProcessScene(const std::string &scenePath, Scene &scene);

    void initEditorScene();
};
} // namespace kirana::scene
#endif