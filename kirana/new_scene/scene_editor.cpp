#include "scene_editor.hpp"

#include <constants.h>

namespace constants = kirana::utils::constants;
using kirana::utils::input::Key;
using kirana::utils::input::KeyAction;
using kirana::utils::input::KeyboardInput;
using kirana::utils::input::ModifierKey;
using kirana::utils::input::MouseButton;
using kirana::utils::input::MouseInput;

void kirana::scene::SceneEditor::resetViewportCamera()
{
    //    const std::vector<Renderable> &renderables =
    //        m_viewportScene.getSelectedRenderables();
    //    if (!renderables.empty())
    //    {
    //        math::Vector3 pivotPos;
    //        math::Bounds3 pivotBounds;
    //
    //        const std::vector<Renderable> &selected =
    //            m_viewportScene.getSelectedRenderables();
    //        if (!selected.empty())
    //        {
    //            for (const auto &r : selected)
    //            {
    //                pivotPos += r.object->transform->getPosition();
    //                pivotBounds.encapsulate(r.object->getHierarchyBounds());
    //            }
    //            pivotPos /= static_cast<float>(selected.size());
    //        }
    //        else
    //        {
    //            pivotPos = m_viewportScene.m_currentScene.getRoot()
    //                           ->transform->getPosition();
    //            pivotBounds =
    //                m_viewportScene.m_currentScene.getRoot()->getHierarchyBounds();
    //        }
    //        m_viewportCamData.pivot.setPosition(pivotPos);
    //        m_viewportCamera.fitBoundsToView(
    //            pivotPos, pivotBounds,
    //            constants::VIEWPORT_CAMERA_DEFAULT_Z_OFFSET);
    //    }
    //    else
    //    {
    //        const math::Vector3 camPos{
    //            constants::VIEWPORT_CAMERA_DEFAULT_POSITION[0],
    //            constants::VIEWPORT_CAMERA_DEFAULT_POSITION[1],
    //            constants::VIEWPORT_CAMERA_DEFAULT_POSITION[2]};
    //        m_viewportCamera.transform.setPosition(camPos);
    //        m_viewportCamData.pivot.setPosition(math::Vector3::ZERO);
    //        m_viewportCamera.lookAt(math::Vector3::ZERO);
    //    }
    //    Logger::get().log(constants::LOG_CHANNEL_VIEWPORT, LogSeverity::trace,
    //                      "Viewport Camera Reset");
}

void kirana::scene::SceneEditor::handleViewportCameraMovement()
{
    //    if (!m_input.getKey(Key::LEFT_ALT))
    //        return;
    //
    //    MouseButton button;
    //    if (m_input.isAnyMouseButtonDown(&button))
    //    {
    //        m_viewportCamData.currentButton = button;
    //        if (m_viewportCamData.firstClick)
    //        {
    //            m_viewportCamData.prevMousePos = m_input.getMousePosition();
    //            m_viewportCamData.mousePos = m_input.getMousePosition();
    //            m_viewportCamData.mouseDelta = math::Vector2::ZERO;
    //            m_viewportCamData.firstClick = false;
    //        }
    //        else
    //        {
    //            m_viewportCamData.prevMousePos = m_viewportCamData.mousePos;
    //            m_viewportCamData.mousePos = m_input.getMousePosition();
    //            m_viewportCamData.mouseDelta =
    //                m_viewportCamData.mousePos -
    //                m_viewportCamData.prevMousePos;
    //            m_viewportCamData.mouseDelta[0] /=
    //                static_cast<float>(m_viewportCamera.windowResolution[0]);
    //            m_viewportCamData.mouseDelta[1] /=
    //                static_cast<float>(m_viewportCamera.windowResolution[1]);
    //        }
    //
    //        switch (button)
    //        {
    //        case MouseButton::LEFT: {
    //            // Camera Orbit
    //            float xAngle = 180.0f * m_viewportCamData.mouseDelta[1];
    //            float yAngle = -360.0f * m_viewportCamData.mouseDelta[0];
    //
    //            xAngle *= constants::VIEWPORT_CAMERA_MOUSE_SENSITIVITY * 0.1f;
    //            yAngle *= constants::VIEWPORT_CAMERA_MOUSE_SENSITIVITY * 0.1f;
    //
    //            const math::Vector3 pivotPos =
    //                m_viewportCamData.pivot.getPosition();
    //            const math::Vector3 camPos =
    //                m_viewportCamera.transform.getPosition();
    //
    //            math::Vector3 lookPos =
    //                (math::Quaternion::angleAxis(yAngle, math::Vector3::UP) *
    //                 (camPos - pivotPos)) +
    //                pivotPos;
    //            lookPos =
    //                (math::Quaternion::angleAxis(xAngle, math::Vector3::RIGHT)
    //                *
    //                 (lookPos - pivotPos)) +
    //                pivotPos;
    //
    //            m_viewportCamera.transform.setPosition(lookPos);
    //            m_viewportCamera.lookAt(pivotPos, math::Vector3::UP);
    //
    //            const math::Quaternion lookDir =
    //            math::Quaternion::lookAtDirection(
    //                math::Vector3::normalize(camPos - pivotPos),
    //                math::Vector3::UP);
    //            m_viewportCamData.pivot.setRotation(lookDir);
    //        }
    //        break;
    //        case MouseButton::MIDDLE: {
    //            // Camera Pan
    //            math::Vector3 translation(-m_viewportCamData.mouseDelta[0],
    //                                      m_viewportCamData.mouseDelta[1],
    //                                      0.0f);
    //            translation *= constants::VIEWPORT_CAMERA_MOUSE_SENSITIVITY;
    //            m_viewportCamera.transform.translateInLocalAxis(translation);
    //            m_viewportCamData.pivot.translateInLocalAxis(translation);
    //        }
    //        break;
    //        case MouseButton::RIGHT: {
    //            // Camera Zoom
    //            float maxDelta = m_viewportCamData.mouseDelta[0];
    //            maxDelta = std::fabsf(maxDelta) <
    //                               std::fabsf(m_viewportCamData.mouseDelta[1])
    //                           ? m_viewportCamData.mouseDelta[1]
    //                           : maxDelta;
    //
    //            const math::Vector3 translation =
    //                -m_viewportCamera.transform.getForward() * maxDelta *
    //                constants::VIEWPORT_CAMERA_MOUSE_SENSITIVITY;
    //            m_viewportCamera.transform.translate(translation);
    //        }
    //        break;
    //        default:
    //            break;
    //        }
    //    }
    //    if (m_inputManager.getMouseUp(m_viewportCamData.currentButton))
    //    {
    //        m_viewportCamData.prevMousePos =
    //        m_inputManager.getMousePosition(); m_viewportCamData.mousePos =
    //        m_inputManager.getMousePosition(); m_viewportCamData.mouseDelta =
    //        math::Vector2::ZERO; m_viewportCamData.firstClick = true;
    //    }
}

void kirana::scene::SceneEditor::checkForObjectSelection(bool multiSelect)
{
    //    if (!m_viewportScene.m_currentScene.isInitialized())
    //        return;
    //    const math::Ray &ray =
    //        m_viewportCamera.screenPositionToRay(m_inputManager.getMousePosition());
    //
    //    // TODO: Write an efficient object selection using BVH
    //    for (size_t i = 0; i <
    //    m_viewportScene.m_currentScene.m_objects.size(); i++)
    //    {
    //        const auto &o = m_viewportScene.m_currentScene.m_objects[i];
    //        if (o->getObjectBounds().intersectWithRay(ray))
    //        {
    //            m_viewportScene.toggleObjectSelection(o->getName(),
    //            multiSelect);
    //            Logger::get().log(constants::LOG_CHANNEL_VIEWPORT,
    //                              LogSeverity::trace,
    //                              "Object Selected: " + o->getName());
    //            return;
    //        }
    //    }
    //
    //    if (!multiSelect)
    //        m_viewportScene.toggleObjectSelection();
}

void kirana::scene::SceneEditor::onKeyboardInput(
    const utils::input::KeyboardInput &input)
{
    if (input.action == KeyAction::DOWN)
    {
        if (input.key == Key::F)
            resetViewportCamera();
    }
}

void kirana::scene::SceneEditor::onMouseInput(
    const utils::input::MouseInput &input)
{
    if (input.action == KeyAction::DOWN && input.modifier != ModifierKey::ALT)
    {
        if (input.button == MouseButton::LEFT)
            checkForObjectSelection((input.modifier & ModifierKey::SHIFT) ==
                                    ModifierKey::SHIFT);
    }
}


void kirana::scene::SceneEditor::init()
{
    m_input.addKeyboardInputListener(
        [&](const utils::input::KeyboardInput &input) {
            onKeyboardInput(input);
        });
    m_input.addMouseInputListener(
        [&](const utils::input::MouseInput &input) { onMouseInput(input); });
    resetViewportCamera();
}
void kirana::scene::SceneEditor::update()
{
    handleViewportCameraMovement();
}

void kirana::scene::SceneEditor::clean()
{
}