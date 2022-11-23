#include "scene_manager.hpp"

#include "scene_importer.hpp"
#include <file_system.hpp>
#include <constants.h>
#include <time.hpp>
#include <input_manager.hpp>

namespace constants = kirana::utils::constants;
using kirana::math::Transform;
using kirana::utils::input::Key;
using kirana ::utils::input::KeyAction;
using kirana::utils::input::ModifierKey;
using kirana::utils::input::MouseButton;
using kirana::utils::input::MouseInput;


void kirana::scene::SceneManager::handleViewportCameraInput()
{
    if (!m_inputManager.getKey(Key::LEFT_ALT))
        return;

    MouseButton button;
    if (m_inputManager.isAnyMouseButtonDown(&button))
    {
        m_viewportCamData.currentButton = button;
        if (m_viewportCamData.firstClick)
        {
            m_viewportCamData.prevMousePos = m_inputManager.getMousePosition();
            m_viewportCamData.mousePos = m_inputManager.getMousePosition();
            m_viewportCamData.mouseDelta = math::Vector2::ZERO;
            m_viewportCamData.firstClick = false;
        }
        else
        {
            m_viewportCamData.prevMousePos = m_viewportCamData.mousePos;
            m_viewportCamData.mousePos = m_inputManager.getMousePosition();
            m_viewportCamData.mouseDelta =
                m_viewportCamData.mousePos - m_viewportCamData.prevMousePos;
        }

        switch (button)
        {
        case MouseButton::LEFT: {
            // Camera Orbit
            // TODO: Add Camera Orbit control (Arc-ball camera).
        }
        break;
        case MouseButton::MIDDLE: {
            // Camera Pan
            math::Vector3 translation(m_viewportCamData.mouseDelta[0],
                                      -m_viewportCamData.mouseDelta[1], 0.0f);
            translation *= constants::VIEWPORT_CAMERA_MOUSE_SENSITIVITY *
                           m_time.getDeltaTime();
            m_currentScene.m_camera.m_transform.translate(translation);
        }
        break;
        case MouseButton::RIGHT: {
            // Camera Zoom
            float maxDelta = m_viewportCamData.mouseDelta[0];
            maxDelta = std::fabsf(maxDelta) <
                               std::fabsf(m_viewportCamData.mouseDelta[1])
                           ? m_viewportCamData.mouseDelta[1]
                           : maxDelta;
            math::Vector3 translation =
                -m_currentScene.m_camera.m_transform.getForward() * maxDelta *
                constants::VIEWPORT_CAMERA_MOUSE_SENSITIVITY *
                m_time.getDeltaTime();
            m_currentScene.m_camera.m_transform.translate(translation);
        }
        break;
        default:
            break;
        }
        m_currentScene.m_camera.m_onCameraChange();
    }
    if (m_inputManager.getMouseUp(m_viewportCamData.currentButton))
    {
        m_viewportCamData.prevMousePos = math::Vector2::ZERO;
        m_viewportCamData.mousePos = math::Vector2::ZERO;
        m_viewportCamData.mouseDelta = math::Vector2::ZERO;
        m_viewportCamData.firstClick = true;
    }
}


kirana::scene::SceneManager::SceneManager()
    : m_inputManager{utils::input::InputManager::get()}, m_time{
                                                             utils::Time::get()}
{
}


kirana::scene::Scene &kirana::scene::SceneManager::loadScene(
    std::string path, const SceneImportSettings &importSettings)
{
    if (path.empty())
        path = utils::filesystem::combinePath(constants::DATA_DIR_PATH,
                                              {constants::DEFAULT_MODEL_NAME});

    scene::SceneImporter::get().loadSceneFromFile(path.c_str(), importSettings,
                                                  &m_currentScene);
    return m_currentScene;
}

void kirana::scene::SceneManager::init()
{
    if (m_currentScene.isInitialized())
    {
        //        m_currentScene.getCamera().transform.translate(
        //            kirana::math::Vector3(0.0f, 1.5f, 3.0f));
        m_currentScene.m_camera.m_transform.translate(
            kirana::math::Vector3(0.0f, 4.0f, 10.0f));
        m_currentScene.m_camera.m_transform.lookAt(kirana::math::Vector3::ZERO,
                                                   kirana::math::Vector3::UP);
    }
}

void kirana::scene::SceneManager::update()
{
    handleViewportCameraInput();
    float rotSpeed = 100.0f * m_time.getDeltaTime();
    float moveSpeed = 10.0f * m_time.getDeltaTime();
    if (m_inputManager.getKey(Key::R))
        m_currentScene.m_rootObject->transform->rotateY(rotSpeed);
    if (m_inputManager.getKey(Key::T))
        m_currentScene.m_objects[2]->transform->rotateY(
            rotSpeed, Transform::Space::Local);

    //    if (m_inputManager.getAxis()[0] > 0.0f ||
    //        m_inputManager.getAxis()[1] > 0.0f)
    //        m_currentScene.m_rootObject->transform->translate(
    //            math::Vector3(m_inputManager.getAxis()[0], 0.0f,
    //                          m_inputManager.getAxis()[1]) *
    //            moveSpeed);
}

void kirana::scene::SceneManager::clean()
{
}