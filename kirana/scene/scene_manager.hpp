#ifndef SCENE_MANAGER_HPP
#define SCENE_MANAGER_HPP

#include "scene.hpp"

#include <string>

namespace kirana::scene
{
class SceneManager
{
  private:
    Scene m_currentScene;

    SceneManager() = default;
    ~SceneManager() = default;

  public:
    SceneManager(const SceneManager &manager) = delete;

    static SceneManager &get()
    {
        static SceneManager instance;
        return instance;
    }

    Scene &loadScene(std::string path = "",
                           const SceneImportSettings &importSettings =
                               DEFAULT_SCENE_IMPORT_SETTINGS);
    inline Scene &getCurrentScene()
    {
        return m_currentScene;
    }

    void init();
    void update();
    void clean();
};
} // namespace kirana::scene
#endif