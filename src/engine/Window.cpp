#include "Window.hpp"
#include "Window.hpp"
#include "EngineException.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <functional>


namespace Engine {

    EventDelegate<GLFWwindow *, int, int> Window::onAnyWindowSizeChanged;
    EventDelegate<GLFWwindow *, int, int> Window::onAnyFramebufferSizeChanged;
    EventDelegate<GLFWwindow *, double, double> Window::onAnyCursorPositionChanged;
    EventDelegate<GLFWwindow *, int, int, int> Window::onAnyMouseButtonChanged;
    EventDelegate<GLFWwindow *, double, double> Window::onAnyScrollChanged;
    EventDelegate<GLFWwindow *, int, int, int, int> Window::onAnyKeyChanged;
    EventDelegate<GLFWwindow *, unsigned int> Window::onAnyTextInput;

    bool v_opengl_context_available = false;

    bool Window::windowCreated = false;

    Window::Window(std::string title, int width, int height) : title(title), width(width), height(height) {
        init();
    }

    void Window::init() {

        if (windowCreated) {
            throw EngineException("A window was already created, there can only be one window!");
        }
        windowCreated = true;

        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (window == nullptr) {
            glfwTerminate();
            throw EngineException("Failed to create GLFW window");
        }

        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            throw EngineException("Failed to initialize GLAD");
        }

        setCallbacks();
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
        unsetCallbacks();
        glfwTerminate();
        v_opengl_context_available = false;
    }


    void Window::onWindowSizeChanged(GLFWwindow *window, int width, int height) {

        if (window != this->window)
            return;

        this->width = width;
        this->height = height;

        OnWindowSizeChanged.Fire(width, height);
    }

    void Window::setCallbacks() {
        glfwSetWindowSizeCallback(window, Window::window_size_callback);
        onAnyWindowSizeChangedSubscription = onAnyWindowSizeChanged.Subscribe(
                std::bind(&Window::onWindowSizeChanged, this, std::placeholders::_1, std::placeholders::_2,
                          std::placeholders::_3));

        glfwSetFramebufferSizeCallback(window, Window::framebuffer_size_callback);
        onAnyFramebufferSizeChangedSubscription = onAnyFramebufferSizeChanged.Subscribe(
                std::bind(&Window::onFramebufferSizeChanged, this, std::placeholders::_1, std::placeholders::_2,
                          std::placeholders::_3));

        glfwSetCursorPosCallback(window, Window::cursor_position_callback);
        onAnyCursorPositionChangedSubscription = onAnyCursorPositionChanged.Subscribe(
                std::bind(&Window::onCursorPositionChanged, this, std::placeholders::_1, std::placeholders::_2,
                          std::placeholders::_3));

        glfwSetMouseButtonCallback(window, Window::mouse_button_callback);
        onAnyMouseButtonChangedSubscription = onAnyMouseButtonChanged.Subscribe(
                std::bind(&Window::onMouseButtonChanged, this, std::placeholders::_1, std::placeholders::_2,
                          std::placeholders::_3, std::placeholders::_4));

        glfwSetScrollCallback(window, Window::scroll_callback);
        onAnyScrollChangedSubscription = onAnyScrollChanged.Subscribe(
                std::bind(&Window::onScrollChanged, this, std::placeholders::_1, std::placeholders::_2,
                          std::placeholders::_3));

        glfwSetKeyCallback(window, key_callback);
        onAnyKeyChangedSubscription = onAnyKeyChanged.Subscribe(
                std::bind(&Window::onKeyChanged, this, std::placeholders::_1, std::placeholders::_2,
                          std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));

        glfwSetCharCallback(window, character_callback);
        onAnyTextInputSubscription = onAnyTextInput.Subscribe(
                std::bind(&Window::onTextInput, this, std::placeholders::_1, std::placeholders::_2));


        callbackSet = true;
    }

    void Window::window_size_callback(GLFWwindow *window, int width, int height) {
        Window::onAnyWindowSizeChanged.Fire(window, width, height);
    }

    void Window::unsetCallbacks() {
        if (!callbackSet)
            return;
        onAnyWindowSizeChanged.Unsubscribe(onAnyWindowSizeChangedSubscription);
        onAnyFramebufferSizeChanged.Unsubscribe(onAnyFramebufferSizeChangedSubscription);
        onAnyCursorPositionChanged.Unsubscribe(onAnyCursorPositionChangedSubscription);
        onAnyMouseButtonChanged.Unsubscribe(onAnyMouseButtonChangedSubscription);
        onAnyScrollChanged.Unsubscribe(onAnyScrollChangedSubscription);
        onAnyKeyChanged.Unsubscribe(onAnyKeyChangedSubscription);
        onAnyTextInput.Unsubscribe(onAnyTextInputSubscription);
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
        glfwSetWindowSize(window, width, height);
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

    void Window::framebuffer_size_callback(GLFWwindow *window, int width, int height) {
        onAnyFramebufferSizeChanged.Fire(window, width, height);
    }

    void Window::onFramebufferSizeChanged(GLFWwindow *window, int width, int height) {
        if (window != this->window) return;
        this->framebufferWidth = width;
        this->framebufferHeight = height;
        this->OnFramebufferSizeChanged.Fire(width, height);
    }

    void Window::setCorrectSizeValues() {
        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
        glfwGetWindowSize(window, &width, &height);
    }

    void Window::MainLoop() {
        load();

        while (!glfwWindowShouldClose(window)) {

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
            glfwSwapBuffers(window);

        }

        unload();
    }

    void Window::cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
        onAnyCursorPositionChanged.Fire(window, xpos, ypos);
    }

    void Window::onCursorPositionChanged(GLFWwindow *window, double xpos, double ypos) {
        if (window != this->window) return;
        mousePositionX = xpos;
        mousePositionY = ypos;
        OnCursorPositionChanged.Fire(xpos, ypos);
    }

    double Window::GetMousePositionX() const {
        return mousePositionX;
    }

    double Window::GetMousePositionY() const {
        return mousePositionY;
    }

    void Window::mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
        onAnyMouseButtonChanged.Fire(window, button, action, mods);
    }

    void Window::onMouseButtonChanged(GLFWwindow *window, int button, int action, int mods) {
        if (window != this->window) return;

        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            if (action == GLFW_PRESS) {
                OnMouseDown.Fire(MouseButton::LeftButton);
            } else if (action == GLFW_RELEASE) {
                OnMouseUp.Fire(MouseButton::LeftButton);
            }
        } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            if (action == GLFW_PRESS) {
                OnMouseDown.Fire(MouseButton::RightButton);
            } else if (action == GLFW_RELEASE) {
                OnMouseUp.Fire(MouseButton::RightButton);
            }
        }

    }

    void Window::onScrollChanged(GLFWwindow *window, double xoffset, double yoffset) {
        if (window != this->window) return;
        OnScrollChanged.Fire(xoffset, yoffset);
    }

    void Window::scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
        onAnyScrollChanged.Fire(window, xoffset, yoffset);
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
        acc /= (double) lastFrameTimesLength;
        return 1.0 / acc;
    }

    void Window::onKeyChanged(GLFWwindow *window, int key, int scancode, int action, int mods) {
        if (window != this->window) return;
        if (action == GLFW_PRESS) {
            OnKeyPressed.Fire(key);
        } else if (action == GLFW_RELEASE) {
            OnKeyRelease.Fire(key);
        }
    }

    std::string UnicodeToUTF8(unsigned int codepoint) {
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

    void Window::onTextInput(GLFWwindow *window, unsigned int codepoint) {
        if (window != this->window) return;
        std::string asUtf8 = UnicodeToUTF8(codepoint);
        OnTextInput.Fire(asUtf8);
    }

    void Window::character_callback(GLFWwindow *window, unsigned int codepoint) {
        onAnyTextInput.Fire(window, codepoint);
    }

    void Window::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
        onAnyKeyChanged.Fire(window, key, scancode, action, mods);
    }

    GLFWwindow *Window::GetWindowHandler() {
        return window;
    }


    bool opengl_context_available()
    {
        return v_opengl_context_available;
    }

    int Window::GetScreenWidth() {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        return mode->width;
    }

    int Window::GetScreenHeight(){
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        return mode->height;
    }

}
