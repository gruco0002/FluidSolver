#include <glad/glad.h>

#include "Window.hpp"

#include "Window.hpp"

#include <GLFW/glfw3.h>
#include <functional>


namespace Engine {


    bool v_opengl_context_available = false;

    Window* Window::active_window = nullptr;

    Window::Window(std::string title, int width, int height)
        : title(title), width(width), height(height) {
        if (active_window != nullptr) {
            throw std::invalid_argument("A window was already created, there can only be one window!");
        }
        active_window = this;

        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        glfw_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (glfw_window == nullptr) {
            glfwTerminate();
            throw std::invalid_argument("Failed to create GLFW window");
        }

        glfwMakeContextCurrent(glfw_window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            throw std::invalid_argument("Failed to initialize GLAD");
        }

        set_all_callbacks(glfw_window);
        setCorrectSizeValues();

        currentTime = glfwGetTime();

        // unlock framerate
        glfwSwapInterval(0);


        // Fixing texture stuff
        // If the textures aren't working as expected this line should be changed
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        // If texture data isn't returned as expected this line should be changed
        glPixelStorei(GL_PACK_ALIGNMENT, 1);

        v_opengl_context_available = true;
    }


    Window::~Window() {
        glfwTerminate();
        v_opengl_context_available = false;
        active_window = nullptr;
    }


    void Window::set_all_callbacks(GLFWwindow* window) {
        glfwSetWindowSizeCallback(window, Window::window_size_callback);
        glfwSetFramebufferSizeCallback(window, Window::framebuffer_size_callback);
        glfwSetCursorPosCallback(window, Window::cursor_position_callback);
        glfwSetMouseButtonCallback(window, Window::mouse_button_callback);
        glfwSetScrollCallback(window, Window::scroll_callback);
        glfwSetKeyCallback(window, key_callback);
        glfwSetCharCallback(window, character_callback);
    }


    int Window::GetHeight() {
        return height;
    }

    int Window::GetWidth() {
        return width;
    }

    void Window::SetSize(int width, int height) {
        this->width = width;
        this->height = height;
        glfwSetWindowSize(glfw_window, width, height);
    }

    void Window::SetWidth(int width) {
        SetSize(width, GetHeight());
    }

    void Window::SetHeight(int height) {
        SetSize(GetWidth(), height);
    }

    int Window::GetFramebufferWidth() {
        return framebufferWidth;
    }

    int Window::GetFramebufferHeight() {
        return framebufferHeight;
    }


    void Window::setCorrectSizeValues() {
        glfwGetFramebufferSize(glfw_window, &framebufferWidth, &framebufferHeight);
        glfwGetWindowSize(glfw_window, &width, &height);
    }

    double Window::GetMousePositionX() const {
        return mousePositionX;
    }

    double Window::GetMousePositionY() const {
        return mousePositionY;
    }

    double Window::GetLastFrameTime() const {
        return lastFrameTime;
    }

    double Window::GetFPS() const {
        return 1.0 / lastFrameTime;
    }

    double Window::GetAvgFPS() const {
        double acc = 0.0f;
        for (size_t i = 0; i < lastFrameTimesLength; i++) {
            acc += lastFrameTimes[i];
        }
        acc /= (double)lastFrameTimesLength;
        return 1.0 / acc;
    }

    GLFWwindow* Window::GetWindowHandler() {
        return glfw_window;
    }


    bool opengl_context_available() {
        return v_opengl_context_available;
    }

    int Window::GetScreenWidth() {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        return mode->width;
    }

    int Window::GetScreenHeight() {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        return mode->height;
    }


    void Window::MainLoop() {
        load();

        while (!glfwWindowShouldClose(glfw_window)) {
            // fps calc
            double tmp = glfwGetTime();
            lastFrameTime = tmp - currentTime;
            currentTime = tmp;

            lastFrameTimesIndex = (lastFrameTimesIndex + 1) % lastFrameTimesLength;
            lastFrameTimes[lastFrameTimesIndex] = lastFrameTime;

            // input handling here

            // rendering here
            render();

            glfwPollEvents();
            glfwSwapBuffers(glfw_window);
        }

        unload();
    }

    void Window::window_size_callback(GLFWwindow* window, int width, int height) {
        if (active_window == nullptr)
            return;

        if (window != active_window->glfw_window)
            return;

        active_window->width = width;
        active_window->height = height;

        active_window->onWindowSizeChanged(width, height);
    }

    void Window::onWindowSizeChanged(int width, int height) {
        // nothing to do
    }


    void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
        if (active_window == nullptr)
            return;

        if (window != active_window->glfw_window)
            return;
        active_window->framebufferWidth = width;
        active_window->framebufferHeight = height;
        active_window->onFramebufferSizeChanged(width, height);
    }

    void Window::onFramebufferSizeChanged(int width, int height) {
        // nothing to do
    }


    void Window::cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
        if (active_window == nullptr)
            return;

        if (window != active_window->glfw_window)
            return;
        active_window->mousePositionX = xpos;
        active_window->mousePositionY = ypos;
        active_window->onCursorPositionChanged(xpos, ypos);
    }

    void Window::onCursorPositionChanged(double xpos, double ypos) {
        // nothing to do
    }


    void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
        if (active_window == nullptr)
            return;

        if (window != active_window->glfw_window)
            return;

        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            if (action == GLFW_PRESS) {
                active_window->on_mouse_down(MouseButton::LeftButton);
            } else if (action == GLFW_RELEASE) {
                active_window->on_mouse_up(MouseButton::LeftButton);
            }
        } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            if (action == GLFW_PRESS) {
                active_window->on_mouse_down(MouseButton::RightButton);
            } else if (action == GLFW_RELEASE) {
                active_window->on_mouse_up(MouseButton::RightButton);
            }
        }
    }

    void Window::on_mouse_down(MouseButton button) {
        // nothing to do
    }
    void Window::on_mouse_up(MouseButton button) {
        // nothing to do
    }

    void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
        if (active_window == nullptr)
            return;

        if (window != active_window->glfw_window)
            return;
        active_window->onScrollChanged(xoffset, yoffset);
    }

    void Window::onScrollChanged(double xoffset, double yoffset) {
        // nothing to do
    }

    void Window::character_callback(GLFWwindow* window, unsigned int codepoint) {
        if (active_window == nullptr)
            return;

        if (window != active_window->glfw_window)
            return;
        std::string asUtf8 = unicode_codepoint_as_utf_8_string(codepoint);
        active_window->onTextInput(asUtf8);
    }

    void Window::onTextInput(std::string text) {
        // nothing to do
    }

    void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (active_window == nullptr)
            return;

        if (window != active_window->glfw_window)
            return;
        if (action == GLFW_PRESS) {
            active_window->OnKeyPressed(key);
        } else if (action == GLFW_RELEASE) {
            active_window->OnKeyReleased(key);
        }
    }

    void Window::OnKeyPressed(int key) {
        // nothing to do
    }
    void Window::OnKeyReleased(int key) {
        // nothing to do
    }

    std::string Window::unicode_codepoint_as_utf_8_string(unsigned int codepoint) {
        std::string out;

        if (codepoint <= 0x7f)
            out.append(1, static_cast<char>(codepoint));
        else if (codepoint <= 0x7ff) {
            out.append(1, static_cast<char>(0xc0 | ((codepoint >> 6) & 0x1f)));
            out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
        } else if (codepoint <= 0xffff) {
            out.append(1, static_cast<char>(0xe0 | ((codepoint >> 12) & 0x0f)));
            out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
            out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
        } else {
            out.append(1, static_cast<char>(0xf0 | ((codepoint >> 18) & 0x07)));
            out.append(1, static_cast<char>(0x80 | ((codepoint >> 12) & 0x3f)));
            out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
            out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
        }
        return out;
    }


} // namespace Engine
