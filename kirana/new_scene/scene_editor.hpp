#ifndef KIRANA_SCENE_EDITOR_HPP
#define KIRANA_SCENE_EDITOR_HPP

#include <transform.hpp>
#include <input_manager.hpp>
#include "scene.hpp"

namespace kirana::scene
{
class SceneEditor
{
  public:
    SceneEditor(Scene &editorScene, Scene &currentScene)
        : m_input{utils::input::InputManager::get()},
          m_editorScene{editorScene}, m_currentScene{currentScene},
          m_viewportCamera{m_editorScene.getCamera(0)} {};
    ~SceneEditor() = default;

    void init();
    void update();
    void clean();

  private:
    struct CameraMovementData
    {
        math::Transform pivot;
        math::Vector2 prevMousePos;
        math::Vector2 mousePos;
        math::Vector2 mouseDelta;
        utils::input::MouseButton currentButton =
            utils::input::MouseButton::LEFT;
        bool firstClick = true;
    };

    utils::input::InputManager &m_input;
    Scene &m_editorScene;
    Scene &m_currentScene;
    Camera &m_viewportCamera;

    CameraMovementData m_viewportCamData;

    void resetViewportCamera();
    void handleViewportCameraMovement();
    void checkForObjectSelection(bool multiSelect = false);
    void onKeyboardInput(const utils::input::KeyboardInput &input);
    void onMouseInput(const utils::input::MouseInput &input);
};
} // namespace kirana::scene
#endif