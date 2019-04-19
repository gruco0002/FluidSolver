//
// Created by corbi on 17.04.2019.
//

#include "FluidSolverWindow.hpp"

#include <glad/glad.h>
#include <engine/EngineException.hpp>

void FluidSolverWindow::render() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

FluidSolverWindow::FluidSolverWindow(const std::string &title, int width, int height) : Window(title, width, height) {}

bool FluidSolverWindow::even(int input) {
    if(input < 0)
        throw Engine::EngineException("negative not allowed");
    return input % 2 == 0;
}
