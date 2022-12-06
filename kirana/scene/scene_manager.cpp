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
    if (m_activeSelection != nullptr)
    {
        m_viewportCamData.pivot = *m_activeSelection->transform;
        m_viewportCamera->fitBoundsToView(
            m_viewportCamData.pivot.getPosition(),
            m_activeSelection->getHierarchyBounds(), offset);
    }
    else
    {
        m_viewportCamData.pivot = kirana::scene::Transform();
        m_viewportCamera->transform.translate(offset);
        m_viewportCamera->lookAt(math::Vector3::ZERO);
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
                 static_cast<float>(m_viewportCamera->windowResolution[1])) *
                m_viewportCamData.mouseDelta[1];
            float yAngle =
                (-360.0f /
                 static_cast<float>(m_viewportCamera->windowResolution[0])) *
                m_viewportCamData.mouseDelta[0];

            xAngle *= constants::VIEWPORT_CAMERA_MOUSE_SENSITIVITY *
                      m_time.getDeltaTime() * math::DEGREE_90;
            yAngle *= constants::VIEWPORT_CAMERA_MOUSE_SENSITIVITY *
                      m_time.getDeltaTime() * math::DEGREE_90;

            const math::Vector3 pivotPos =
                m_viewportCamData.pivot.getPosition();
            const math::Vector3 camPos =
                m_viewportCamera->transform.getPosition();

            math::Vector3 lookPos =
                (math::Quaternion::angleAxis(yAngle, math::Vector3::UP) *
                 (camPos - pivotPos)) +
                pivotPos;
            lookPos =
                (math::Quaternion::angleAxis(xAngle, math::Vector3::RIGHT) *
                 (lookPos - pivotPos)) +
                pivotPos;

            m_viewportCamera->transform.setPosition(lookPos);
            m_viewportCamera->lookAt(pivotPos, math::Vector3::UP);

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
            m_viewportCamera->transform.translate(translation,
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

void kirana::scene::SceneManager::checkForObjectSelection()
{
    const math::Ray &ray = m_viewportCamera->screenPositionToRay(
        m_inputManager.getMousePosition());
    // TODO: Write an efficient object selection using BVH
    for (const auto &o : m_currentScene.m_objects)
    {
        if (o->getObjectBounds().intersectWithRay(ray))
        {
            m_activeSelection = o.get();
            Logger::get().log(
                constants::LOG_CHANNEL_VIEWPORT, LogSeverity::trace,
                "Object Selected: " + m_activeSelection->getName());
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
            checkForObjectSelection();
    }
}

kirana::scene::SceneManager::SceneManager()
    : m_viewportCamera{m_currentScene.getActiveCamera()},
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
        m_activeSelection = m_currentScene.getRoot().get();
        resetViewportCamera();
    }
}

void kirana::scene::SceneManager::update()
{
    handleViewportCameraMovement();
}

void kirana::scene::SceneManager::clean()
{
}