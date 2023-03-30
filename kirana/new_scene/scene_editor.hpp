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
          m_editorScene{editorScene}, m_currentScene{currentScene} {};
    ~SceneEditor() = default;

    void init();
    void update();
    void clean();
    inline void setWindowResolution(const math::Vector2 &windowResolution)
    {
        m_windowResolution = windowResolution;
    }

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

    math::Vector2 m_windowResolution;
    CameraMovementData m_viewportCamData;

    math::Transform fitBoundsToCameraView(const math::Transform &camTransform,
                                          float camFOV,
                                          const math::Bounds3 &boundingBox,
                                          const math::Vector3 &pivotPoint,
                                          float distanceOffset);
    void resetViewportCamera();
    void handleViewportCameraMovement();
    void checkForObjectSelection(bool multiSelect = false);
    void onKeyboardInput(const utils::input::KeyboardInput &input);
    void onMouseInput(const utils::input::MouseInput &input);
};
} // namespace kirana::scene
#endif