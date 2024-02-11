#include <glad/glad.h>

#include "Window.hpp"

#include "Window.hpp"

#include <GLFW/glfw3.h>
#include <functional>

#include <stdexcept>

namespace Engine
{

    bool v_opengl_context_available = false;

    Window *Window::active_window = nullptr;

    Window::Window(const std::string &title) : title(title)
    {
        if (active_window != nullptr)
        {
            throw std::invalid_argument("A window was already created, there can only be one window!");
        }
        active_window = this;

        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        glfw_window = glfwCreateWindow(800, 600, title.c_str(), nullptr, nullptr);
        if (glfw_window == nullptr)
        {
            glfwTerminate();
            throw std::invalid_argument("Failed to create GLFW window");
        }
        glfwMaximizeWindow(glfw_window);

        glfwMakeContextCurrent(glfw_window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            throw std::invalid_argument("Failed to initialize GLAD");
        }

        set_all_callbacks(glfw_window);
        set_correct_size_values();

        current_time = glfwGetTime();

        // unlock framerate
        glfwSwapInterval(0);

        // Fixing texture stuff
        // If the textures aren't working as expected this line should be changed
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        // If texture data isn't returned as expected this line should be changed
        glPixelStorei(GL_PACK_ALIGNMENT, 1);

        v_opengl_context_available = true;
    }

    Window::~Window()
    {
        glfwTerminate();
        v_opengl_context_available = false;
        active_window = nullptr;
    }

    void Window::set_all_callbacks(GLFWwindow *window)
    {
        glfwSetWindowSizeCallback(window, Window::window_size_callback);
        glfwSetFramebufferSizeCallback(window, Window::framebuffer_size_callback);
        glfwSetCursorPosCallback(window, Window::cursor_position_callback);
        glfwSetMouseButtonCallback(window, Window::mouse_button_callback);
        glfwSetScrollCallback(window, Window::scroll_callback);
        glfwSetKeyCallback(window, key_callback);
        glfwSetCharCallback(window, character_callback);
    }

    int Window::get_height()
    {
        return height;
    }

    int Window::get_width()
    {
        return width;
    }

    void Window::set_size(int width, int height)
    {
        this->width = width;
        this->height = height;
        glfwSetWindowSize(glfw_window, width, height);
    }

    void Window::set_width(int width)
    {
        set_size(width, get_height());
    }

    void Window::set_height(int height)
    {
        set_size(get_width(), height);
    }

    int Window::get_framebuffer_width()
    {
        return framebuffer_width;
    }

    int Window::get_framebuffer_height()
    {
        return framebuffer_height;
    }

    void Window::set_correct_size_values()
    {
        glfwGetFramebufferSize(glfw_window, &framebuffer_width, &framebuffer_height);
        glfwGetWindowSize(glfw_window, &width, &height);
    }

    double Window::get_mouse_position_x() const
    {
        return mouse_position_x;
    }

    double Window::get_mouse_position_y() const
    {
        return mouse_position_y;
    }

    double Window::get_last_frame_time() const
    {
        return last_frame_time;
    }

    double Window::get_fps() const
    {
        return 1.0 / last_frame_time;
    }

    double Window::get_average_fps() const
    {
        double acc = 0.0f;
        for (size_t i = 0; i < last_frame_times_length; i++)
        {
            acc += last_frame_times[i];
        }
        acc /= (double)last_frame_times_length;
        return 1.0 / acc;
    }

    GLFWwindow *Window::get_window_handler()
    {
        return glfw_window;
    }

    bool opengl_context_available()
    {
        return v_opengl_context_available;
    }

    int Window::get_screen_width()
    {
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        return mode->width;
    }

    int Window::get_screen_height()
    {
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        return mode->height;
    }

    void Window::main_loop()
    {
        load();

        while (!glfwWindowShouldClose(glfw_window))
        {
            // fps calc
            double tmp = glfwGetTime();
            last_frame_time = tmp - current_time;
            current_time = tmp;

            last_frame_times_index = (last_frame_times_index + 1) % last_frame_times_length;
            last_frame_times[last_frame_times_index] = last_frame_time;

            // input handling here

            // rendering here
            render();

            glfwPollEvents();
            glfwSwapBuffers(glfw_window);
        }

        unload();
    }

    void Window::window_size_callback(GLFWwindow *window, int width, int height)
    {
        if (active_window == nullptr)
            return;

        if (window != active_window->glfw_window)
            return;

        active_window->width = width;
        active_window->height = height;

        active_window->on_window_size_changed(width, height);
    }

    void Window::on_window_size_changed(int width, int height)
    {
        // nothing to do
    }

    void Window::framebuffer_size_callback(GLFWwindow *window, int width, int height)
    {
        if (active_window == nullptr)
            return;

        if (window != active_window->glfw_window)
            return;
        active_window->framebuffer_width = width;
        active_window->framebuffer_height = height;
        active_window->on_framebuffer_size_changed(width, height);
    }

    void Window::on_framebuffer_size_changed(int width, int height)
    {
        // nothing to do
    }

    void Window::cursor_position_callback(GLFWwindow *window, double xpos, double ypos)
    {
        if (active_window == nullptr)
            return;

        if (window != active_window->glfw_window)
            return;
        active_window->mouse_position_x = xpos;
        active_window->mouse_position_y = ypos;
        active_window->on_cursor_position_changed(xpos, ypos);
    }

    void Window::on_cursor_position_changed(double xpos, double ypos)
    {
        // nothing to do
    }

    void Window::mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
    {
        if (active_window == nullptr)
            return;

        if (window != active_window->glfw_window)
            return;

        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            if (action == GLFW_PRESS)
            {
                active_window->on_mouse_down(MouseButton::LeftButton);
            }
            else if (action == GLFW_RELEASE)
            {
                active_window->on_mouse_up(MouseButton::LeftButton);
            }
        }
        else if (button == GLFW_MOUSE_BUTTON_RIGHT)
        {
            if (action == GLFW_PRESS)
            {
                active_window->on_mouse_down(MouseButton::RightButton);
            }
            else if (action == GLFW_RELEASE)
            {
                active_window->on_mouse_up(MouseButton::RightButton);
            }
        }
    }

    void Window::on_mouse_down(MouseButton button)
    {
        // nothing to do
    }
    void Window::on_mouse_up(MouseButton button)
    {
        // nothing to do
    }

    void Window::scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
    {
        if (active_window == nullptr)
            return;

        if (window != active_window->glfw_window)
            return;
        active_window->on_scroll_changed(xoffset, yoffset);
    }

    void Window::on_scroll_changed(double xoffset, double yoffset)
    {
        // nothing to do
    }

    void Window::character_callback(GLFWwindow *window, unsigned int codepoint)
    {
        if (active_window == nullptr)
            return;

        if (window != active_window->glfw_window)
            return;
        std::string asUtf8 = unicode_codepoint_as_utf_8_string(codepoint);
        active_window->on_text_input(asUtf8);
    }

    void Window::on_text_input(std::string text)
    {
        // nothing to do
    }

    void Window::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        if (active_window == nullptr)
            return;

        if (window != active_window->glfw_window)
            return;
        if (action == GLFW_PRESS)
        {
            active_window->on_key_pressed(key);
        }
        else if (action == GLFW_RELEASE)
        {
            active_window->on_key_released(key);
        }
    }

    void Window::on_key_pressed(int key)
    {
        // nothing to do
    }
    void Window::on_key_released(int key)
    {
        // nothing to do
    }

    std::string Window::unicode_codepoint_as_utf_8_string(unsigned int codepoint)
    {
        std::string out;

        if (codepoint <= 0x7f)
            out.append(1, static_cast<char>(codepoint));
        else if (codepoint <= 0x7ff)
        {
            out.append(1, static_cast<char>(0xc0 | ((codepoint >> 6) & 0x1f)));
            out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
        }
        else if (codepoint <= 0xffff)
        {
            out.append(1, static_cast<char>(0xe0 | ((codepoint >> 12) & 0x0f)));
            out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
            out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
        }
        else
        {
            out.append(1, static_cast<char>(0xf0 | ((codepoint >> 18) & 0x07)));
            out.append(1, static_cast<char>(0x80 | ((codepoint >> 12) & 0x3f)));
            out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
            out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
        }
        return out;
    }

} // namespace Engine
