#pragma once

#include <core/Core.hpp>
#include <ui/FrameInput.hpp>

struct GLFWwindow;

namespace jaccen
{
    class IWindowEventListener;

    class Window : public trait::no_copy_or_move
    {
    public:
        using WindowResizeCallback = std::function<void(int2)>;

    public:
        Window(int width, int height, const std::string& title);
        ~Window();
        
        // Poll and handle events (input, window resizing, etc.)
        // Returns false if window should be closed.
        bool pollEvents();

        void beginFrame();
        void endFrame();

        // Add an input and message event listener to this window.
        void addEventListener(std::shared_ptr<IWindowEventListener> listener);

        void onWindowResize(WindowResizeCallback callback);

        // The height and width of this window.
        int2 size() const;

        // Capture the mouse cursor in this window.
        void captureMouseCursor(bool capture);

        // Cached input state of current frame.
        const FrameInput& frameInput() const { return m_inputState; }

    private:
        static void onKeyInput(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void onMouseButton(GLFWwindow* window, int button, int action, int mods);
        static void onMouseCursor(GLFWwindow* window, double xpos, double ypos);
        static void onMouseScroll(GLFWwindow* window, double xoffset, double yoffset);
        static void onFramebufferSizeCallback(GLFWwindow* window, int width, int height);

    private:
        GLFWwindow* m_window = nullptr;

        int2 m_windowSize = { 0, 0 };
        FrameInput m_inputState;

        std::vector<WindowResizeCallback> m_windowResizeCallbacks;
        std::vector<std::shared_ptr<IWindowEventListener>> m_eventListeners;
    };

    struct InputEvent
    {
        int key = 0;
        int action = 0;
        int mods = 0;
    };

    // Window input and message event listener.
    // New events are only received at the start of each frame.
    class IWindowEventListener
    {
    public:
        // todo: replace direct input callbacks with processed 'KeyBindings'

        virtual void onKeyInput(Window&, const InputEvent&) {}
        virtual void onMouseButton(Window&, const InputEvent&) {}
        virtual void onMouseCursor(Window&, double2) {}
        virtual void onMouseScroll(Window&, double2) {}
        virtual void onWindowResize(Window&, int2) {}
    };
}
