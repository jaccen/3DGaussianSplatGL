#include <ui/FrameInput.hpp>

namespace jaccen
{
    void FrameInput::beginFrame()
    {
        // Cleanup key and mouse button frame state
        m_keys.pressed.reset();
        m_keys.repeated &= ~m_keys.released;
        m_keys.released.reset();

        m_buttons.pressed.reset();
        m_buttons.repeated &= ~m_buttons.released;
        m_buttons.released.reset();

        // Cleanup cursor movement
        cursorMoved = false;
    }

    void FrameInput::setKey(int key, int action)
    {
        if (key == GLFW_KEY_UNKNOWN)
        {
            return;
        }

        if (action == GLFW_RELEASE)
        {
            m_keys.released.set(key);
            m_keys.repeated.set(key, false);
        }
        else if (action == GLFW_PRESS)
        {
            m_keys.pressed.set(key);
            m_keys.repeated.set(key);
        }
        else if (action == GLFW_REPEAT)
        {
            // We have no need for this information at the moment
        }
    }

    bool FrameInput::isKeyPressed(int key) const
    {
        return m_keys.pressed.test(key);
    }

    bool FrameInput::isKeyReleased(int key) const
    {
        return m_keys.released.test(key);
    }

    bool FrameInput::isKeyDown(int key) const
    {
        return m_keys.pressed.test(key) || m_keys.repeated.test(key);
    }

    void FrameInput::setButton(int button, int action)
    {
        if (action == GLFW_RELEASE)
        {
            m_buttons.released.set(button);
            m_buttons.repeated.set(button, false);
        }
        else if (action == GLFW_PRESS)
        {
            m_buttons.pressed.set(button);
            m_buttons.repeated.set(button);
        }

        // GLFW_REPEAT doesn't exist for buttons
    }

    bool FrameInput::isButtonPressed(int button) const
    {
        return m_buttons.pressed.test(button);
    }

    bool FrameInput::isButtonReleased(int button) const
    {
        return m_buttons.released.test(button);
    }

    bool FrameInput::isButtonDown(int button) const
    {
        return m_buttons.pressed.test(button) || m_buttons.repeated.test(button);
    }
}