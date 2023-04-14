#include "scene_editor.hpp"

#include <constants.h>
#include <logger.hpp>
#include <math_utils.hpp>

namespace constants = kirana::utils::constants;
using kirana::utils::Logger;
using kirana::utils::LogSeverity;
using kirana::utils::input::Key;
using kirana::utils::input::KeyAction;
using kirana::utils::input::KeyboardInput;
using kirana::utils::input::ModifierKey;
using kirana::utils::input::MouseButton;
using kirana::utils::input::MouseInput;

kirana::math::Transform kirana::scene::SceneEditor::fitBoundsToCameraView(
    const math::Transform &camTransform, float camFOV,
    const math::Bounds3 &boundingBox, const math::Vector3 &pivotPoint,
    float distanceOffset)
{
    const float extent = boundingBox.getExtent().length();
    float distance = extent / std::sinf(math::radians(camFOV * 0.5f));
    distance += distanceOffset;

    math::Vector3 camPos = camTransform.getPosition();
    const float distBetweenCamObject =
        (boundingBox.getCenter() - camPos).length();

    camPos += camTransform.getForward() * (distance - distBetweenCamObject);
    math::Transform newTransform;
    newTransform.setPosition(camPos);
    newTransform.lookAt(pivotPoint, math::Vector3::UP);
    return newTransform;
}

void kirana::scene::SceneEditor::resetViewportCamera()
{
    const std::vector<Node> &selectedNodes = m_currentScene.getSelectedNodes();
    // Get the editor viewport camera
    auto &cam = m_editorScene.getCameras()[0];
    auto &camNode = m_editorScene.getNodeByName(cam.getName());
    auto camTransform = m_editorScene.getGlobalTransform(camNode);
    if (selectedNodes.empty())
    {
        const math::Vector3 camPos{
            constants::VIEWPORT_CAMERA_DEFAULT_POSITION[0],
            constants::VIEWPORT_CAMERA_DEFAULT_POSITION[1],
            constants::VIEWPORT_CAMERA_DEFAULT_POSITION[2]};
        camTransform.setPosition(camPos);
        m_viewportCamData.pivot.setPosition(math::Vector3::ZERO);
        camTransform.lookAt(math::Vector3::ZERO, math::Vector3::UP);
    }
    else
    {
        math::Vector3 pivotPos;
        math::Bounds3 pivotBounds;
        for (const auto &n : selectedNodes)
        {
            pivotPos += m_editorScene.getGlobalTransform(n).getPosition();
            pivotBounds.encapsulate(m_editorScene.getMeshAtNode(n).getBounds());
        }
        pivotPos /= static_cast<float>(selectedNodes.size());

        m_viewportCamData.pivot.setPosition(pivotPos);
        camTransform = fitBoundsToCameraView(
            camTransform, cam.getProperties().fieldOfView, pivotBounds,
            pivotPos, constants::VIEWPORT_CAMERA_DEFAULT_Z_OFFSET);
    }

    m_editorScene.setGlobalTransform(camNode, camTransform);
    Logger::get().log(constants::LOG_CHANNEL_VIEWPORT, LogSeverity::trace,
                      "Viewport Camera Reset");
}

void kirana::scene::SceneEditor::handleViewportCameraMovement()
{
    if (!m_input.getKey(Key::LEFT_ALT))
        return;

    MouseButton button;
    if (m_input.isAnyMouseButtonDown(&button))
    {
        auto &cam = m_editorScene.getCameras()[0];
        auto camTransform = m_editorScene.getGlobalTransform(cam.getName());

        m_viewportCamData.currentButton = button;
        if (m_viewportCamData.firstClick)
        {
            m_viewportCamData.prevMousePos = m_input.getMousePosition();
            m_viewportCamData.mousePos = m_input.getMousePosition();
            m_viewportCamData.mouseDelta = math::Vector2::ZERO;
            m_viewportCamData.firstClick = false;
        }
        else
        {
            m_viewportCamData.prevMousePos = m_viewportCamData.mousePos;
            m_viewportCamData.mousePos = m_input.getMousePosition();
            m_viewportCamData.mouseDelta =
                m_viewportCamData.mousePos - m_viewportCamData.prevMousePos;
            m_viewportCamData.mouseDelta[0] /= m_windowResolution[0];
            m_viewportCamData.mouseDelta[1] /= m_windowResolution[1];
        }

        switch (button)
        {
        case MouseButton::LEFT: {
            // Camera Orbit
            float xAngle = 180.0f * m_viewportCamData.mouseDelta[1];
            float yAngle = -360.0f * m_viewportCamData.mouseDelta[0];

            xAngle *= constants::VIEWPORT_CAMERA_MOUSE_SENSITIVITY * 0.1f;
            yAngle *= constants::VIEWPORT_CAMERA_MOUSE_SENSITIVITY * 0.1f;

            const math::Vector3 pivotPos =
                m_viewportCamData.pivot.getPosition();
            const math::Vector3 camPos = camTransform.getPosition();

            math::Vector3 lookPos =
                (math::Quaternion::angleAxis(yAngle, math::Vector3::UP) *
                 (camPos - pivotPos)) +
                pivotPos;
            lookPos =
                (math::Quaternion::angleAxis(xAngle, math::Vector3::RIGHT) *
                 (lookPos - pivotPos)) +
                pivotPos;

            camTransform.setPosition(lookPos);
            camTransform.lookAt(pivotPos, math::Vector3::UP);

            const math::Quaternion lookDir = math::Quaternion::lookAtDirection(
                math::Vector3::normalize(camPos - pivotPos), math::Vector3::UP);
            m_viewportCamData.pivot.setRotation(lookDir);
        }
        break;
        case MouseButton::MIDDLE: {
            // Camera Pan
            math::Vector3 translation(-m_viewportCamData.mouseDelta[0],
                                      m_viewportCamData.mouseDelta[1], 0.0f);
            translation *= constants::VIEWPORT_CAMERA_MOUSE_SENSITIVITY;
            camTransform.translateInLocalAxis(translation);
            m_viewportCamData.pivot.translateInLocalAxis(translation);
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
                -camTransform.getForward() * maxDelta *
                constants::VIEWPORT_CAMERA_MOUSE_SENSITIVITY;
            camTransform.translate(translation);
        }
        break;
        default:
            break;
        }
        m_editorScene.setGlobalTransform(cam.getName(), camTransform);
    }
    if (m_input.getMouseUp(m_viewportCamData.currentButton))
    {
        m_viewportCamData.prevMousePos = m_input.getMousePosition();
        m_viewportCamData.mousePos = m_input.getMousePosition();
        m_viewportCamData.mouseDelta = math::Vector2::ZERO;
        m_viewportCamData.firstClick = true;
    }
}

void kirana::scene::SceneEditor::checkForObjectSelection(bool multiSelect)
{
    auto &cam = m_editorScene.getCameras()[0];
    const math::Ray &ray =
        cam.screenPositionToRay(m_input.getMousePosition(), m_windowResolution);

    Node selectedNode;
    // TODO: Write an efficient and precise object selection using BVH
    // Find the mesh node which is selected
    for (const auto &m : m_currentScene.getMeshes())
    {
        const auto &node = m_currentScene.getNodeByName(m.getName());
        if (!node.renderData.selectable)
            continue;
        if (m.getBounds().intersectWithRay(ray))
        {
            selectedNode = node;
            break;
        }
    }
    // Loop over all the meshes again to toggle selection
    for (const auto &m : m_currentScene.getMeshes())
    {
        const auto &node = m_currentScene.getNodeByName(m.getName());
        auto renderData = node.renderData;
        if (node.index == selectedNode.index)
        {
            renderData.selected = !renderData.selected;
            m_currentScene.setNodeRenderData(node, renderData);
        }
        // If multi-select is off, deselect all the other meshes.
        else if (!multiSelect)
        {
            renderData.selected = false;
            m_currentScene.setNodeRenderData(node, renderData);
        }
    }
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
            checkForObjectSelection(input.modifier & ModifierKey::SHIFT);
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