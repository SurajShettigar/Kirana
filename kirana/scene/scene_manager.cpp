#include "scene_manager.hpp"

#include "scene_importer.hpp"
#include <file_system.hpp>
#include <constants.h>
#include <time.hpp>
#include <input_manager.hpp>
#include <math_utils.hpp>

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
            math::Vector2 delta = m_viewportCamData.mouseDelta;
            delta[0] = m_viewportCamData.mouseDelta[0] /
                       static_cast<float>(m_viewportCamera.windowResolution[0]);
            delta[1] = m_viewportCamData.mouseDelta[1] /
                       static_cast<float>(m_viewportCamera.windowResolution[1]);

            float xAngle = 180.0f * delta[1];
            float yAngle = -360.0f * delta[0];

            if (xAngle > 89.0f)
                xAngle = 89.0f;
            else if (xAngle < -89.0f)
                xAngle = -89.0f;

            math::Vector3 lookPos =
                (math::Quaternion::angleAxis(xAngle, math::Vector3::RIGHT) *
                 (m_viewportCamera.transform.getPosition() -
                  m_viewportCamera.pivot)) +
                m_viewportCamera.pivot;
            lookPos = (math::Quaternion::angleAxis(yAngle, math::Vector3::UP) *
                       (lookPos - m_viewportCamera.pivot)) +
                      m_viewportCamera.pivot;

            m_viewportCamera.transform.setPosition(lookPos);
            m_viewportCamera.lookAt(m_viewportCamera.pivot);
        }
        break;
        case MouseButton::MIDDLE: {
            // Camera Pan
            math::Vector3 translation(-m_viewportCamData.mouseDelta[0],
                                      m_viewportCamData.mouseDelta[1], 0.0f);
            translation *= constants::VIEWPORT_CAMERA_MOUSE_SENSITIVITY *
                           m_time.getDeltaTime() *
                           m_viewportCamera.transform.getPosition()[2];
            m_viewportCamera.pivot += translation;
            m_viewportCamera.transform.translate(translation,
                                                 Transform::Space::World);
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
                -m_viewportCamera.transform.getForward() * maxDelta *
                constants::VIEWPORT_CAMERA_MOUSE_SENSITIVITY *
                m_time.getDeltaTime() *
                m_viewportCamera.transform.getPosition()[2];

            m_viewportCamera.transform.translate(translation);
        }
        break;
        default:
            break;
        }
    }
    if (m_inputManager.getMouseUp(m_viewportCamData.currentButton))
    {
        m_viewportCamData.prevMousePos = m_inputManager.getMousePosition();
        m_viewportCamData.mousePos = m_inputManager.getMousePosition();
        m_viewportCamData.mouseDelta = math::Vector2::ZERO;
        m_viewportCamData.firstClick = true;
    }
}


kirana::scene::SceneManager::SceneManager()
    : m_viewportCamera{m_currentScene.m_camera},
      m_inputManager{utils::input::InputManager::get()}, m_time{
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
    m_viewportCamera = m_currentScene.m_camera;
    return m_currentScene;
}

void kirana::scene::SceneManager::init()
{
    if (m_currentScene.isInitialized())
    {
        //        m_currentScene.getCamera().transform.translate(
        //            kirana::math::Vector3(0.0f, 1.5f, 3.0f));
        m_viewportCamera.transform.translate(
            kirana::math::Vector3(0.0f, 5.0f, 10.0f));
        m_viewportCamera.lookAt(
            m_currentScene.m_rootObject->transform->getPosition(),
            kirana::math::Vector3::UP);
    }
}

void kirana::scene::SceneManager::update()
{
    handleViewportCameraInput();
    float rotSpeed = 100.0f * m_time.getDeltaTime();
    float moveSpeed = 10.0f * m_time.getDeltaTime();

    if (m_inputManager.getKey(Key::R))
        m_currentScene.m_rootObject->transform->rotateY(
            rotSpeed, Transform::Space::Local);
    if (m_inputManager.getKey(Key::E))
        m_currentScene.m_rootObject->transform->rotateX(
            rotSpeed, Transform::Space::Local);
    if (m_inputManager.getKey(Key::T))
        m_currentScene.m_objects[2]->transform->rotateY(
            rotSpeed, Transform::Space::Local);

    if (std::fabsf(m_inputManager.getAxis()[0]) > 0.0f ||
        std::fabsf(m_inputManager.getAxis()[1]) > 0.0f)
        m_currentScene.m_rootObject->transform->translate(
            math::Vector3(m_inputManager.getAxis()[0], 0.0f,
                          m_inputManager.getAxis()[1]) *
                moveSpeed,
            Transform::Space::Local);

    if (m_inputManager.getKeyDown(Key::F))
    {
        m_viewportCamera.transform.setPosition(
            kirana::math::Vector3(0.0f, 5.0f, 10.0f));
        m_viewportCamera.lookAt(
            m_currentScene.m_rootObject->transform->getPosition(),
            math::Vector3::UP);
    }

    if (m_inputManager.getKeyDown(Key::L))
    {
        math::Vector3 dir =
            m_viewportCamera.transform.getPosition() -
            m_currentScene.m_rootObject->transform->getPosition();
        dir.normalize();
        m_currentScene.m_rootObject->transform->lookAt(dir, math::Vector3::UP);
    }
}

void kirana::scene::SceneManager::clean()
{
}