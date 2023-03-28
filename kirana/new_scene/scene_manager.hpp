#ifndef KIRANA_SCENE_SCENE_MANAGER_HPP
#define KIRANA_SCENE_SCENE_MANAGER_HPP

#include "scene_types.hpp"
#include "scene.hpp"

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

    bool loadScene(const std::string &scenePath,
                   const SceneImportSettings &settings = {});
    bool loadDefaultScene();

    [[nodiscard]] const Scene &getEditorScene() const
    {
        return m_editorScene;
    }
    [[nodiscard]] const Scene &getCurrentScene() const
    {
        return m_currentScene;
    }

  private:
    Scene m_editorScene;

    std::string m_currentScenePath;
    Scene m_currentScene;

    SceneManager()
    {
        initEditorScene();
    }
    ~SceneManager() = default;

    static void postProcessScene(const std::string &scenePath, Scene &scene);

    void initEditorScene();
};
} // namespace kirana::scene
#endif