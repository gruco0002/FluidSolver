//
// Created by corbi on 16.04.2019.
//

#include "Window.hpp"
#include "EngineException.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <functional>


namespace Engine {

    EventDelegate<GLFWwindow *, int, int> Window::onAnyWindowSizeChanged;
    EventDelegate<GLFWwindow *, int, int> Window::onAnyFramebufferSizeChanged;

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
    }

    Window::~Window() {
        unsetCallbacks();
        glfwTerminate();
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

        while (!glfwWindowShouldClose(window)) {

            // input handling here

            // rendering here
            render();

            glfwPollEvents();
            glfwSwapBuffers(window);

        }

    }

}
