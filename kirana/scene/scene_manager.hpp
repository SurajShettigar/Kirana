#ifndef SCENE_MANAGER_HPP
#define SCENE_MANAGER_HPP

#include "scene.hpp"

#include <string>

namespace kirana::scene
{
class SceneManager
{
  private:
    uint32_t m_frameNum;
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

    const Scene &loadScene(std::string path = "",
                     const SceneImportSettings &importSettings =
                         DEFAULT_SCENE_IMPORT_SETTINGS);
    inline const Scene &getCurrentScene() const
    {
        return m_currentScene;
    }

    void init();
    void update();
    void clean();
};
} // namespace kirana::scene
#endif