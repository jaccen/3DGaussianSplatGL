#include <ui/Window.hpp>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <ImGuizmo.h>

using namespace jaccen;

namespace
{
    inline static Window* getUserWindow(GLFWwindow* window)
    {
        return static_cast<Window*>(glfwGetWindowUserPointer(window));
    }

    void onGlfwError(int error, const char* description)
    {
        ECS_LOG_ERROR("%s [GLFW #%d]", description, error);
    }
}

Window::Window(int width, int height, const std::string& title)
{
    glfwSetErrorCallback(onGlfwError);
    if (!glfwInit())
    {
        assert(false && "Failed to initialize GLFW.");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    m_window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    assert(m_window && "Failed to create OpenGL context.");
    m_windowSize = { width, height };
    
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1); // Enable vsync

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        assert(false && "Failed to initialize GLAD OpenGL loader.");
    }

    glfwSetWindowUserPointer(m_window, this);
    glfwSetKeyCallback(m_window, onKeyInput);
    glfwSetMouseButtonCallback(m_window, onMouseButton);
    glfwSetCursorPosCallback(m_window, onMouseCursor);
    glfwSetScrollCallback(m_window, onMouseScroll);
    glfwSetFramebufferSizeCallback(m_window, onFramebufferSizeCallback);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    // Note: "true" steals input from GLFW
    if (!ImGui_ImplGlfw_InitForOpenGL(m_window, true))
    {
        assert(false && "ImGui failed to initialize window.");
    }

    //const char* glsl_version = "#version 330"; // core
    if (!ImGui_ImplOpenGL3_Init(nullptr))
    {
        assert(false && "ImGui failed to initialize OpenGL.");
    }
}

Window::~Window()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwSetWindowUserPointer(m_window, nullptr);
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

bool Window::pollEvents()
{
    if (glfwWindowShouldClose(m_window))
    {
        return false;
    }

    m_inputState.beginFrame();
    glfwPollEvents();

    return true;
}

void Window::beginFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    // static bool open = true;
    // if (open) { ::ImGui::ShowDemoWindow(&open); }
}

void Window::endFrame()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(::ImGui::GetDrawData());

    glfwSwapBuffers(m_window);

    // NOTE: Now this includes both logic and rendering
    jaccen::Time::endFrame(glfwGetTime());
}

void Window::captureMouseCursor(bool capture)
{
    m_inputState.cursorCaptured = capture;

    glfwSetInputMode(m_window, GLFW_CURSOR, m_inputState.cursorCaptured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void Window::addEventListener(std::shared_ptr<IWindowEventListener> listener)
{
    m_eventListeners.emplace_back(listener);

    listener->onWindowResize(*this, size());
}

void Window::onWindowResize(WindowResizeCallback callback)
{
    m_windowResizeCallbacks.emplace_back(callback);
}

void Window::onKeyInput(GLFWwindow* window, int key, int /*scancode*/, int action, int mods)
{
    auto w = getUserWindow(window);
    w->m_inputState.setKey(key, action);

    for (auto& l : w->m_eventListeners)
    {
        l->onKeyInput(*w, { key, action, mods });
    }
}

void Window::onMouseButton(GLFWwindow* window, int button, int action, int mods)
{
    auto w = getUserWindow(window);
    w->m_inputState.setButton(button, action); // Pass mods?

    for (auto& l : w->m_eventListeners)
    {
        l->onMouseButton(*w, { button, action, mods });
    }
}

void Window::onMouseCursor(GLFWwindow* window, double xpos, double ypos)
{
    auto w = getUserWindow(window);

    double x = (2.0f * xpos) / w->m_windowSize.x - 1.0f;
    double y = 1.0f - (2.0f * ypos) / w->m_windowSize.y;

    w->m_inputState.cursorMoved = true;
    w->m_inputState.cursorPosition = { xpos, ypos };
    w->m_inputState.cursorPositionNormalized = { x, y };

    for (auto& l : w->m_eventListeners)
    {
        l->onMouseCursor(*w, w->m_inputState.cursorPosition);
    }
}

void Window::onMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
    auto w = getUserWindow(window);

    for (auto& l : w->m_eventListeners)
    {
        l->onMouseScroll(*w, { xoffset, yoffset });
    }
}

void Window::onFramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);

    auto w = getUserWindow(window);
    w->m_windowSize = { width, height };

    for (auto& l : w->m_eventListeners)
    {
        l->onWindowResize(*w, w->m_windowSize);
    }

    for (auto& c : w->m_windowResizeCallbacks)
    {
        c(w->m_windowSize);
    }
}

int2 Window::size() const
{
    return m_windowSize;
}
