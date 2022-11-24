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
            //            auto fromV = static_cast<math::Vector3>(
            //                m_viewportCamData.normalizedPrevMousePos(
            //                    m_viewportCamera.m_windowResolution));
            //            auto toV =
            //                static_cast<math::Vector3>(m_viewportCamData.normalizedMousePos(
            //                    m_viewportCamera.m_windowResolution));
            //            // Flip Y-axis
            //            fromV[1] = -fromV[1];
            //            toV[1] = -toV[1];
            //            // TODO: Calculate vectors with the current object as
            //            origin. fromV = math::Vector3::spherical(
            //                fromV, 1.0f);
            //            toV = math::Vector3::spherical(
            //                toV, 1.0f);
            //            m_viewportCamera.m_transform.rotate(
            //                math::Quaternion::rotationFromVectors(fromV,
            //                toV));
            math::Vector3 pivot = m_currentScene.m_rootObject->transform->getPosition();
            float xAngle = static_cast<float>(
                               2.0 * math::PI /
                               static_cast<double>(
                                   m_viewportCamera.m_windowResolution[0])) *
                           m_viewportCamData.mouseDelta[0] * 50.0f;
            float yAngle =
                static_cast<float>(
                    math::PI / static_cast<double>(
                                   m_viewportCamera.m_windowResolution[1])) *
                m_viewportCamData.mouseDelta[1] * 50.0f;
            if (std::fabsf(math::Vector3::dot(
                    m_viewportCamera.m_transform.getForward(),
                    math::Vector3::UP)) >= 0.9999f)
                yAngle = 0.0f;
            math::Vector3 pos =
                math::Quaternion::angleAxis(xAngle, math::Vector3::UP) *
                    (m_viewportCamera.m_transform.getPosition() - pivot) +
                pivot;
            pos = math::Quaternion::angleAxis(yAngle, math::Vector3::RIGHT) *
                      (pos - pivot) +
                  pivot;

            m_viewportCamera.m_transform.setPosition(pos);
            m_viewportCamera.m_transform.lookAt(pivot, math::Vector3::UP);
        }
        break;
        case MouseButton::MIDDLE: {
            // Camera Pan
            math::Vector3 translation(m_viewportCamData.mouseDelta[0],
                                      -m_viewportCamData.mouseDelta[1], 0.0f);
            translation *= constants::VIEWPORT_CAMERA_MOUSE_SENSITIVITY *
                           m_time.getDeltaTime();
            m_viewportCamera.m_transform.translate(translation);
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
                -m_viewportCamera.m_transform.getForward() * maxDelta *
                constants::VIEWPORT_CAMERA_MOUSE_SENSITIVITY *
                m_time.getDeltaTime();
            m_viewportCamera.m_transform.translate(translation);
        }
        break;
        default:
            break;
        }
        m_viewportCamera.m_onCameraChange();
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
        m_viewportCamera.m_transform.translate(
            kirana::math::Vector3(0.0f, 5.0f, 10.0f));
        m_viewportCamera.m_transform.lookAt(kirana::math::Vector3::ZERO,
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
            rotSpeed, Transform::Space::World);
    if (m_inputManager.getKey(Key::E))
        m_currentScene.m_rootObject->transform->rotateX(
            rotSpeed, Transform::Space::World);
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

    if (m_inputManager.getKeyDown(Key::SPACE))
    {
        m_viewportCamera.m_transform.lookAt(math::Vector3::ZERO,
                                            math::Vector3::UP);
        m_viewportCamera.m_onCameraChange();
    }

    if (m_inputManager.getKeyDown(Key::L))
    {
        math::Vector3 dir =
            m_viewportCamera.m_transform.getPosition() -
            m_currentScene.m_rootObject->transform->getPosition();
        dir.normalize();
        m_currentScene.m_rootObject->transform->setRotation(
            math::Quaternion::lookAtDirection(dir, math::Vector3::UP)
                .getEulerAngles());
    }
}

void kirana::scene::SceneManager::clean()
{
}