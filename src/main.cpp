#include <iostream>
#include <engine/Window.hpp>

int main()
{
    std::cout << "Hello World!" << std::endl;
    Engine::Window window("testing");

    window.MainLoop();
}