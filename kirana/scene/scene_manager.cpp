#include "scene_manager.hpp"

#include "scene_importer.hpp"
#include <file_system.hpp>
#include <logger.hpp>
#include <constants.h>
#include <time.hpp>
#include <input_manager.hpp>
#include <math_utils.hpp>

namespace constants = kirana::utils::constants;
using kirana::math::Transform;
using kirana::utils::Logger;
using kirana::utils::LogSeverity;
using kirana::utils::input::Key;
using kirana ::utils::input::KeyAction;
using kirana::utils::input::ModifierKey;
using kirana::utils::input::MouseButton;
using kirana::utils::input::MouseInput;


void kirana::scene::SceneManager::resetViewportCamera()
{
    const math::Vector3 offset{constants::VIEWPORT_CAMERA_DEFAULT_OFFSET[0],
                               constants::VIEWPORT_CAMERA_DEFAULT_OFFSET[1],
                               constants::VIEWPORT_CAMERA_DEFAULT_OFFSET[2]};
    if (!m_viewportScene.m_selectedObjects.empty())
    {
        math::Vector3 pivotPos;
        math::Bounds3 pivotBounds;
        for (const auto &i : m_viewportScene.m_selectedObjects)
        {
            const auto &o = m_viewportScene.m_currentScene.m_objects[i];
            pivotPos += o->transform->getPosition();
            pivotBounds.encapsulate(o->getHierarchyBounds());
        }
        pivotPos /=
            static_cast<float>(m_viewportScene.m_selectedObjects.size());
        m_viewportCamera.fitBoundsToView(pivotPos, pivotBounds, offset);
    }
    else
    {
        m_viewportCamData.pivot.setPosition(math::Vector3::ZERO);
        m_viewportCamera.transform.translate(offset);
        m_viewportCamera.lookAt(math::Vector3::ZERO);
    }
    Logger::get().log(constants::LOG_CHANNEL_VIEWPORT, LogSeverity::trace,
                      "Viewport Camera Reset");
}

void kirana::scene::SceneManager::handleViewportCameraMovement()
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
                 static_cast<float>(m_viewportCamera.windowResolution[1])) *
                m_viewportCamData.mouseDelta[1];
            float yAngle =
                (-360.0f /
                 static_cast<float>(m_viewportCamera.windowResolution[0])) *
                m_viewportCamData.mouseDelta[0];

            xAngle *= constants::VIEWPORT_CAMERA_MOUSE_SENSITIVITY *
                      m_time.getDeltaTime() * math::DEGREE_90;
            yAngle *= constants::VIEWPORT_CAMERA_MOUSE_SENSITIVITY *
                      m_time.getDeltaTime() * math::DEGREE_90;

            const math::Vector3 pivotPos =
                m_viewportCamData.pivot.getPosition();
            const math::Vector3 camPos =
                m_viewportCamera.transform.getPosition();

            math::Vector3 lookPos =
                (math::Quaternion::angleAxis(yAngle, math::Vector3::UP) *
                 (camPos - pivotPos)) +
                pivotPos;
            lookPos =
                (math::Quaternion::angleAxis(xAngle, math::Vector3::RIGHT) *
                 (lookPos - pivotPos)) +
                pivotPos;

            m_viewportCamera.transform.setPosition(lookPos);
            m_viewportCamera.lookAt(pivotPos, math::Vector3::UP);

            const math::Quaternion lookDir = math::Quaternion::lookAtDirection(
                math::Vector3::normalize(camPos - pivotPos), math::Vector3::UP);
            m_viewportCamData.pivot.setRotation(lookDir);
        }
        break;
        case MouseButton::MIDDLE: {
            // Camera Pan
            math::Vector3 translation(-m_viewportCamData.mouseDelta[0],
                                      m_viewportCamData.mouseDelta[1], 0.0f);
            translation *= constants::VIEWPORT_CAMERA_MOUSE_SENSITIVITY *
                           m_time.getDeltaTime();
            m_viewportCamera.transform.translate(translation,
                                                 Transform::Space::Local);
            m_viewportCamData.pivot.translate(translation,
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

            const math::Vector3 translation =
                -m_viewportCamera.transform.getForward() * maxDelta *
                constants::VIEWPORT_CAMERA_MOUSE_SENSITIVITY *
                m_time.getDeltaTime();
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

void kirana::scene::SceneManager::checkForObjectSelection(bool multiSelect)
{
    if (!m_viewportScene.m_currentScene.isInitialized())
        return;

    if (!multiSelect)
        m_viewportScene.m_selectedObjects.clear();

    const math::Ray &ray =
        m_viewportCamera.screenPositionToRay(m_inputManager.getMousePosition());

    // TODO: Write an efficient object selection using BVH
    for (size_t i = 0; i < m_viewportScene.m_currentScene.m_objects.size(); i++)
    {
        const auto &o = m_viewportScene.m_currentScene.m_objects[i];
        if (o->getObjectBounds().intersectWithRay(ray))
        {
            m_viewportScene.m_selectedObjects.push_back(i);
            Logger::get().log(constants::LOG_CHANNEL_VIEWPORT,
                              LogSeverity::trace,
                              "Object Selected: " + o->getName());
            return;
        }
    }
}

void kirana::scene::SceneManager::onKeyboardInput(
    const utils::input::KeyboardInput &input)
{
    if (input.action == KeyAction::DOWN)
    {
        if (input.key == Key::F)
            resetViewportCamera();
    }
}

void kirana::scene::SceneManager::onMouseInput(
    const utils::input::MouseInput &input)
{
    if (input.action == KeyAction::DOWN)
    {
        if (input.button == MouseButton::LEFT)
            checkForObjectSelection((input.modifier & ModifierKey::SHIFT) ==
                                    ModifierKey::SHIFT);
    }
}

kirana::scene::SceneManager::SceneManager()
    : m_viewportScene{ViewportScene::get()},
      m_viewportCamera{*m_viewportScene.m_camera},
      m_inputManager{utils::input::InputManager::get()}, m_time{
                                                             utils::Time::get()}
{
    m_inputManager.addKeyboardInputListener(
        [&](const utils::input::KeyboardInput &input) {
            onKeyboardInput(input);
        });
    m_inputManager.addMouseInputListener(
        [&](const utils::input::MouseInput &input) { onMouseInput(input); });
}

bool kirana::scene::SceneManager::loadScene(
    std::string path, const SceneImportSettings &importSettings)
{
    if (path.empty())
        path = utils::filesystem::combinePath(constants::DATA_DIR_PATH,
                                              {constants::DEFAULT_MODEL_NAME});

    scene::SceneImporter::get().loadSceneFromFile(
        path.c_str(), importSettings, &m_viewportScene.m_currentScene);

    m_viewportScene.onSceneLoaded();
    return m_viewportScene.m_currentScene.isInitialized();
}

void kirana::scene::SceneManager::init()
{
    resetViewportCamera();
}

void kirana::scene::SceneManager::update()
{
    handleViewportCameraMovement();
}

void kirana::scene::SceneManager::clean()
{
}