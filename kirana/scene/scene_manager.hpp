#ifndef SCENE_MANAGER_HPP
#define SCENE_MANAGER_HPP

#include "viewport_scene.hpp"

#include <input_manager.hpp>
#include <time.hpp>
#include <string>

namespace kirana::scene
{
class SceneManager
{
  private:
    struct ViewportCameraData
    {
        math::TransformHierarchy pivot;
        math::Vector2 prevMousePos;
        math::Vector2 mousePos;
        math::Vector2 mouseDelta;
        utils::input::MouseButton currentButton;
        bool firstClick = true;
    };

    utils::input::InputManager &m_inputManager;
    utils::Time &m_time;


    ViewportScene &m_viewportScene;
    Camera &m_viewportCamera;
    ViewportCameraData m_viewportCamData;

    void resetViewportCamera();
    void handleViewportCameraMovement();
    void checkForObjectSelection(bool multiSelect = false);
    void onKeyboardInput(const utils::input::KeyboardInput &input);
    void onMouseInput(const utils::input::MouseInput &input);
    SceneManager();
    ~SceneManager() = default;

  public:
    SceneManager(const SceneManager &manager) = delete;

    static SceneManager &get()
    {
        static SceneManager instance;
        return instance;
    }

    bool loadScene(std::string path = "",
                           const SceneImportSettings &importSettings =
                               DEFAULT_SCENE_IMPORT_SETTINGS);

    inline ViewportScene &getViewportScene()
    {
        return m_viewportScene;
    }

    void init();
    void update();
    void clean();
};
} // namespace kirana::scene
#endif