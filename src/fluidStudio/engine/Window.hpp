#pragma once

#include <string>

typedef struct GLFWwindow GLFWwindow;

namespace Engine {

    bool opengl_context_available();

    class Window {
      public:
        enum MouseButton {
            RightButton,
            LeftButton
        };

      public:
        Window(std::string title, int width = 800, int height = 600);


        int GetHeight();

        int GetWidth();

        void SetSize(int width, int height);

        void SetWidth(int width);

        void SetHeight(int height);

        int GetFramebufferWidth();

        int GetFramebufferHeight();

        int GetScreenWidth();

        int GetScreenHeight();

        void MainLoop();

      protected:
        virtual void render() = 0;

        virtual void load() = 0;

        virtual void unload() = 0;

        GLFWwindow* GetWindowHandler();

      private:
        std::string title;

        int width;
        int height;

        double currentTime;
        double lastFrameTime;

        const static unsigned int lastFrameTimesLength = 50;
        unsigned int lastFrameTimesIndex = 0;
        double lastFrameTimes[lastFrameTimesLength] = {0};

      public:
        double GetLastFrameTime() const;

        double GetFPS() const;

        double GetAvgFPS() const;

      protected:
        virtual void onWindowSizeChanged(int width, int height);
        virtual void onFramebufferSizeChanged(int width, int height);
        virtual void onCursorPositionChanged(double xpos, double ypos);
        virtual void on_mouse_down(MouseButton button);
        virtual void on_mouse_up(MouseButton button);
        virtual void onScrollChanged(double xoffset, double yoffset);
        virtual void OnKeyPressed(int key);
        virtual void OnKeyReleased(int key);
        virtual void onTextInput(std::string text);


      public:
        double GetMousePositionX() const;

        double GetMousePositionY() const;

      private:
        double mousePositionX;
        double mousePositionY;

        int framebufferWidth;
        int framebufferHeight;

        GLFWwindow* glfw_window;

        void setCorrectSizeValues();


      private:
        static void set_all_callbacks(GLFWwindow* window);

        // callbacks for glfw
        static void window_size_callback(GLFWwindow* window, int width, int height);

        static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

        static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

        static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

        static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

        static void character_callback(GLFWwindow* window, unsigned int codepoint);

        static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

        static std::string unicode_codepoint_as_utf_8_string(unsigned int codepoint);


      private:
        // only one window can exist
        static Window* active_window;


      public:
        virtual ~Window();
    };
} // namespace Engine
