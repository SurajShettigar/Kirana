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
            float xAngle =
                (180.0f /
                 static_cast<float>(m_viewportCamera->windowResolution[1])) *
                m_viewportCamData.mouseDelta[1];
            float yAngle =
                (-360.0f /
                 static_cast<float>(m_viewportCamera->windowResolution[0])) *
                m_viewportCamData.mouseDelta[0];

            xAngle *= constants::VIEWPORT_CAMERA_MOUSE_SENSITIVITY *
                      m_time.getDeltaTime() * 100.0f;
            yAngle *= constants::VIEWPORT_CAMERA_MOUSE_SENSITIVITY *
                      m_time.getDeltaTime() * 100.0f;

            if (std::fabsf(
                    math::Vector3::dot(m_viewportCamera->transform.getForward(),
                                       math::Vector3::UP)) > 0.9f)
                xAngle = 0.0f;

            math::Vector3 pivotPos = m_viewportCamData.pivot.getPosition();

            math::Vector3 lookPos =
                (math::Quaternion::angleAxis(yAngle, math::Vector3::UP) *
                 (m_viewportCamera->transform.getPosition() - pivotPos)) +
                pivotPos;
            lookPos =
                (math::Quaternion::angleAxis(xAngle, math::Vector3::RIGHT) *
                 (lookPos - pivotPos)) +
                pivotPos;

            m_viewportCamera->transform.setPosition(lookPos);
            m_viewportCamera->lookAt(pivotPos, math::Vector3::UP);
        }
        break;
        case MouseButton::MIDDLE: {
            // Camera Pan
            math::Vector3 translation(m_viewportCamData.mouseDelta[0],
                                      -m_viewportCamData.mouseDelta[1], 0.0f);
            translation *= constants::VIEWPORT_CAMERA_MOUSE_SENSITIVITY *
                           m_time.getDeltaTime();
            m_viewportCamData.pivot.translate(translation,
                                              Transform::Space::Local);
            m_viewportCamera->transform.translate(translation,
                                                  Transform::Space::Local);
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
                -m_viewportCamera->transform.getForward() * maxDelta *
                constants::VIEWPORT_CAMERA_MOUSE_SENSITIVITY *
                m_time.getDeltaTime();
            m_viewportCamera->transform.translate(translation);
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
    : m_viewportCamera{m_currentScene.getActiveCamera()},
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
    return m_currentScene;
}

void kirana::scene::SceneManager::init()
{
    if (m_currentScene.isInitialized())
    {
        //        m_currentScene.getCamera().transform.translate(
        //            kirana::math::Vector3(0.0f, 1.5f, 3.0f));
        m_viewportCamera->transform.translate(
            kirana::math::Vector3(0.0f, 0.0f, 10.0f));
        m_viewportCamData.pivot = *m_currentScene.m_rootObject->transform;
        m_viewportCamera->lookAt(math::Vector3::ZERO);
    }
}

void kirana::scene::SceneManager::update()
{
    handleViewportCameraInput();
    const float rotSpeed = 100.0f * m_time.getDeltaTime();
    const float moveSpeed = 10.0f * m_time.getDeltaTime();
    static float dirVal = 1.0f;
    if (m_inputManager.getKey(Key::I))
        dirVal += m_time.getDeltaTime();
    if (m_inputManager.getKey(Key::U))
        dirVal -= m_time.getDeltaTime();

    if (m_inputManager.getMouseDown(MouseButton::LEFT))
    {
        math::Vector3 screenPos =
            math::Vector3(m_inputManager.getMousePosition()[0],
                          m_inputManager.getMousePosition()[1], 1.0f);

        math::Ray ray = m_viewportCamera->screenPositionToRay(screenPos);
//        std::cout << "Ray: " << ray << std::endl;

        if (m_currentScene.m_objects[2]
                ->transform
                ->transformBounds(
                    m_currentScene.m_objects[2]->getMeshes()[0]->getBounds())
                .intersectWithRay(ray))
        {
            std::cout << "Hit: " << m_currentScene.m_objects[2]->getName()
                      << std::endl;
        }
    }

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
        m_viewportCamera->transform.setPosition(
            kirana::math::Vector3(0.0f, 0.0f, 10.0f));
        m_viewportCamData.pivot = *m_currentScene.m_rootObject->transform;
        m_viewportCamera->lookAt(math::Vector3::ZERO);
    }

    if (m_inputManager.getKeyDown(Key::L))
    {
        math::Vector3 dir =
            m_viewportCamera->transform.getPosition() -
            m_currentScene.m_rootObject->transform->getPosition();
        dir.normalize();
        m_currentScene.m_rootObject->transform->lookAt(dir, math::Vector3::UP);
    }
}

void kirana::scene::SceneManager::clean()
{
}