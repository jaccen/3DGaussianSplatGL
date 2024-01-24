#include <scene/CameraController.hpp>

using namespace jaccen;

CameraController::CameraController(int2 initialViewportSize)
{
    m_lastX = static_cast<double>(initialViewportSize.x) / 2;
    m_lastY = static_cast<double>(initialViewportSize.y) / 2;
}

bool CameraController::update(CameraControl& control)
{
    if (m_captured)
    { 
        // If camera should be moving, keep updating state
        m_updated |= m_movement != CameraMovement::None;

        auto offsetX = m_offsetX * control.sensitivity;
        auto offsetY = m_offsetY * control.sensitivity;

        control.movement = m_movement;
        control.yaw += offsetX;
        control.pitch += offsetY;

        math::clamp(control.pitch, -89.0f, 89.0f);
    }
    
    if (m_updated)
    {
        // Zooming doesn't require mouse capture
        control.zoom = m_zoom;
    }

    // Clear frame specific mouse input state
    m_offsetX = 0.0f;
    m_offsetY = 0.0f;
    m_zoom = 0.0f;

    return std::exchange(m_updated, false);
}

void CameraController::onKeyInput(Window& window, const InputEvent& input)
{
    auto updateMovement = [&](int key, CameraMovement movement)
    {
        if (input.key == key)
        {
            if (input.action != GLFW_RELEASE)
            {
                m_movement |= movement;
            }
            else
            {
                m_movement &= ~movement;

                // Ensure that we report CameraMovement::None as
                // a movement update, when all keys are released
                if (m_movement == CameraMovement::None)
                { 
                    m_updated = true;
                }
            }
        }
    };

    updateMovement(GLFW_KEY_W, CameraMovement::Forward);
    updateMovement(GLFW_KEY_S, CameraMovement::Backward);
    updateMovement(GLFW_KEY_A, CameraMovement::Left);
    updateMovement(GLFW_KEY_D, CameraMovement::Right);

    // Alternative control mode for touch pads
    if (input.key == GLFW_KEY_C)
    {
        activate(input.action == GLFW_PRESS || input.action == GLFW_REPEAT, window);
    }
}

void CameraController::onMouseButton(Window& window, const InputEvent& input)
{
    if (input.key == GLFW_MOUSE_BUTTON_RIGHT)
    {
        activate(input.action == GLFW_PRESS, window);
    }
}

void CameraController::onMouseCursor(Window&, double2 screenPosition)
{
    if (m_firstMouse)
    {
        m_lastX = screenPosition.x;
        m_lastY = screenPosition.y;
        m_firstMouse = false;
    }

    // Reversed y-coordinates since range is from bottom to top
    double offsetX = screenPosition.x - m_lastX;
    double offsetY = m_lastY - screenPosition.y;

    // Store screen position always to keep offset calculation up-to-date
    m_lastX = screenPosition.x;
    m_lastY = screenPosition.y;

    if (!m_captured)
    {
        return;
    }

    // When toggling mouse cursor capture, GLFW does its own saving and restoring of
    // the cursor position. The position appears to jump incorrectly in some cases right
    // after mouse capture is enabled. To prevent this from wildly changing our camera
    // rotation, we ignore the one inaccurate offset calculation after the change.

    if (m_capturedCursorDelay > 0)
    {
        m_capturedCursorDelay--;
        return;
    }

    m_offsetX = static_cast<float>(offsetX);
    m_offsetY = static_cast<float>(offsetY);
    m_updated = true;
}

void CameraController::onMouseScroll(Window&, double2 offset)
{
    m_zoom = static_cast<float>(offset.y);
    m_updated = true;
}

void CameraController::activate(bool condition, Window& window)
{
    m_captured = condition;
    m_capturedCursorDelay = m_captured ? 1 : 0;

    window.captureMouseCursor(m_captured);
}