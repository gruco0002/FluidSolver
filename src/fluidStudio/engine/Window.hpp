#pragma once

#include <string>

typedef struct GLFWwindow GLFWwindow;

namespace Engine
{

    bool opengl_context_available();

    class Window
    {
      public:
        enum MouseButton
        {
            RightButton,
            LeftButton
        };

      public:
        explicit Window(const std::string &title);

        virtual ~Window();

        int get_height();

        int get_width();

        void set_size(int width, int height);

        void set_width(int width);

        void set_height(int height);

        int get_framebuffer_width();

        int get_framebuffer_height();

        int get_screen_width();

        int get_screen_height();

        double get_mouse_position_x() const;

        double get_mouse_position_y() const;

        double get_last_frame_time() const;

        double get_fps() const;

        double get_average_fps() const;

        void main_loop();

      protected:
        virtual void render() = 0;
        virtual void load() = 0;
        virtual void unload() = 0;

        GLFWwindow *get_window_handler();

        virtual void on_window_size_changed(int width, int height);
        virtual void on_framebuffer_size_changed(int width, int height);
        virtual void on_cursor_position_changed(double xpos, double ypos);
        virtual void on_mouse_down(MouseButton button);
        virtual void on_mouse_up(MouseButton button);
        virtual void on_scroll_changed(double xoffset, double yoffset);
        virtual void on_key_pressed(int key);
        virtual void on_key_released(int key);
        virtual void on_text_input(std::string text);

      private:
        std::string title;

        int width;
        int height;

        double current_time;
        double last_frame_time;

        const static unsigned int last_frame_times_length = 50;
        unsigned int last_frame_times_index = 0;
        double last_frame_times[last_frame_times_length] = {0};

        double mouse_position_x;
        double mouse_position_y;

        int framebuffer_width;
        int framebuffer_height;

        GLFWwindow *glfw_window;

        void set_correct_size_values();

      private:
        // only one window can exist
        static Window *active_window;

        static void set_all_callbacks(GLFWwindow *window);

        // callbacks for glfw
        static void window_size_callback(GLFWwindow *window, int width, int height);

        static void framebuffer_size_callback(GLFWwindow *window, int width, int height);

        static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);

        static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);

        static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

        static void character_callback(GLFWwindow *window, unsigned int codepoint);

        static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

        static std::string unicode_codepoint_as_utf_8_string(unsigned int codepoint);
    };
} // namespace Engine
