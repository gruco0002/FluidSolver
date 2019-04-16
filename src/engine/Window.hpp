//
// Created by corbi on 16.04.2019.
//

#ifndef FLUIDSOLVER_WINDOW_HPP
#define FLUIDSOLVER_WINDOW_HPP


#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "EventDelegate.hpp"

namespace Engine {

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
void onFramebufferSizeChanged(GLFWwindow* window, int width, int height);

        static EventDelegate<GLFWwindow*, int, int> onAnyWindowSizeChanged;
        uint32_t onAnyWindowSizeChangedSubscription;
        static EventDelegate<GLFWwindow*, int, int> onAnyFramebufferSizeChanged;
        uint32_t onAnyFramebufferSizeChangedSubscription;


        // callbacks for glfw
        static void window_size_callback(GLFWwindow *window, int width, int height);
        static void framebuffer_size_callback(GLFWwindow *window, int width, int height);


    public:
        virtual ~Window();

    };
}

#endif //FLUIDSOLVER_WINDOW_HPP
