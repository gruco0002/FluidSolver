//
// Created by corbi on 16.04.2019.
//

#ifndef CPPGUI_DEMO_WINDOW_HPP
#define CPPGUI_DEMO_WINDOW_HPP


#include <string>

#include "EventDelegate.hpp"

typedef struct GLFWwindow GLFWwindow;

namespace Engine {

    /**
     * There can only be one window created.
     */
    class Window {

    public:
        enum MouseButton {
            RightButton,
            LeftButton
        };

    public:
        Window(std::string title, int width = 800, int height = 600);

        EventDelegate<int, int> OnWindowSizeChanged;
        EventDelegate<int, int> OnFramebufferSizeChanged;
        EventDelegate<double, double> OnCursorPositionChanged;
        EventDelegate<MouseButton> OnMouseDown;
        EventDelegate<MouseButton> OnMouseUp;
        EventDelegate<double, double> OnScrollChanged;
        EventDelegate<int> OnKeyPressed;
        EventDelegate<int> OnKeyRelease;
        EventDelegate<std::string> OnTextInput;

        int GetHeight();

        int GetWidth();

        void SetSize(int width, int height);

        void SetWidth(int width);

        void SetHeight(int height);

        int GetFramebufferWidth();

        int GetFramebufferHeight();

        void MainLoop();

    protected:

        virtual void render() = 0;

        virtual void load() = 0;

        virtual void unload() = 0;

        GLFWwindow *GetWindowHandler();

    private:
        void init();

        bool callbackSet = false;

        void setCallbacks();

        void unsetCallbacks();

        std::string title;

        int width;
        int height;

        double currentTime;
        double lastFrameTime;

		const static unsigned int lastFrameTimesLength = 50;
		unsigned int lastFrameTimesIndex = 0;
		double lastFrameTimes[lastFrameTimesLength] = { 0 };

    public:
        double GetLastFrameTime() const;

        double GetFPS() const;

		double GetAvgFPS() const;

    public:
        double GetMousePositionX() const;

        double GetMousePositionY() const;

    private:
        double mousePositionX;
        double mousePositionY;

        int framebufferWidth;
        int framebufferHeight;

        GLFWwindow *window;

        void setCorrectSizeValues();


        void onWindowSizeChanged(GLFWwindow *window, int width, int height);

        void onFramebufferSizeChanged(GLFWwindow *window, int width, int height);

        void onCursorPositionChanged(GLFWwindow *window, double xpos, double ypos);

        void onMouseButtonChanged(GLFWwindow *window, int button, int action, int mods);

        void onScrollChanged(GLFWwindow *window, double xoffset, double yoffset);

        void onKeyChanged(GLFWwindow* window, int key, int scancode, int action, int mods);

        void onTextInput(GLFWwindow* window, unsigned int codepoint);


        static EventDelegate<GLFWwindow *, int, int> onAnyWindowSizeChanged;
        uint32_t onAnyWindowSizeChangedSubscription;
        static EventDelegate<GLFWwindow *, int, int> onAnyFramebufferSizeChanged;
        uint32_t onAnyFramebufferSizeChangedSubscription;
        static EventDelegate<GLFWwindow *, double, double> onAnyCursorPositionChanged;
        uint32_t onAnyCursorPositionChangedSubscription;
        static EventDelegate<GLFWwindow *, int, int, int> onAnyMouseButtonChanged;
        uint32_t onAnyMouseButtonChangedSubscription;
        static EventDelegate<GLFWwindow *, double, double> onAnyScrollChanged;
        uint32_t onAnyScrollChangedSubscription;
        static EventDelegate<GLFWwindow*, int, int, int, int> onAnyKeyChanged;
        uint32_t onAnyKeyChangedSubscription;
        static EventDelegate<GLFWwindow*, unsigned  int> onAnyTextInput;
        uint32_t onAnyTextInputSubscription;


        // callbacks for glfw
        static void window_size_callback(GLFWwindow *window, int width, int height);

        static void framebuffer_size_callback(GLFWwindow *window, int width, int height);

        static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);

        static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);

        static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

        static void character_callback(GLFWwindow* window, unsigned int codepoint);

        static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);


        // only one window can exist
        static bool windowCreated;


    public:
        virtual ~Window();

    };
}

#endif //CPPGUI_DEMO_WINDOW_HPP
