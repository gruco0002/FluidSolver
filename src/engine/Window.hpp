//
// Created by corbi on 16.04.2019.
//

#ifndef FLUIDSOLVER_WINDOW_HPP
#define FLUIDSOLVER_WINDOW_HPP


#include <string>

#include "EventDelegate.hpp"

typedef struct GLFWwindow GLFWwindow;

namespace Engine {

    /**
     * There can only be one window created.
     */
    class Window {

    public:
        Window(std::string title, int width = 800, int height = 600);

        EventDelegate<int, int> OnWindowSizeChanged;
        EventDelegate<int, int> OnFramebufferSizeChanged;

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

    private:
        void init();

        bool callbackSet = false;

        void setCallbacks();

        void unsetCallbacks();

        std::string title;

        int width;
        int height;

        int framebufferWidth;
        int framebufferHeight;

        GLFWwindow *window;

        void setCorrectSizeValues();


        void onWindowSizeChanged(GLFWwindow *window, int width, int height);

        void onFramebufferSizeChanged(GLFWwindow *window, int width, int height);

        static EventDelegate<GLFWwindow *, int, int> onAnyWindowSizeChanged;
        uint32_t onAnyWindowSizeChangedSubscription;
        static EventDelegate<GLFWwindow *, int, int> onAnyFramebufferSizeChanged;
        uint32_t onAnyFramebufferSizeChangedSubscription;


        // callbacks for glfw
        static void window_size_callback(GLFWwindow *window, int width, int height);

        static void framebuffer_size_callback(GLFWwindow *window, int width, int height);


        // only one window can exist
        static bool windowCreated;


    public:
        virtual ~Window();

    };
}

#endif //FLUIDSOLVER_WINDOW_HPP
