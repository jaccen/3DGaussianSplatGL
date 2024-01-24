#pragma once

#include <core/Core.hpp>

#include <bitset>

namespace jaccen
{ 
    // Frame specific cached mouse and keyboard input state.
    class FrameInput
    {
    public:
        // Mouse cursor position in pixel coordinates.
        double2 cursorPosition = { 0.0f, 0.0f };
        // Mouse cursor position in normalized device coordinates within range [-1, 1].
        double2 cursorPositionNormalized = { 0.0f, 0.0f };

        // Whether mouse cursor moved this frame.
        bool cursorMoved = false;
        // Whether mouse cursor is captured by the window.
        bool cursorCaptured = false;

    public:
        // Maintain input state cache. 
        // Should be called each frame before setting new input state.
        void beginFrame();

        void setKey(int key, int action);
        bool isKeyPressed(int key) const;
        bool isKeyReleased(int key) const;
        bool isKeyDown(int key) const;

        void setButton(int button, int action);
        bool isButtonPressed(int button) const;
        bool isButtonReleased(int button) const;
        bool isButtonDown(int button) const;

    private:
        struct KeyCodes
        {
            std::bitset<GLFW_KEY_LAST + 1> released;
            std::bitset<GLFW_KEY_LAST + 1> pressed;
            std::bitset<GLFW_KEY_LAST + 1> repeated;
        };

        KeyCodes m_keys;
        KeyCodes m_buttons;
    };
}