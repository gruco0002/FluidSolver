//
// Created by corbi on 17.04.2019.
//

#include "FluidSolverWindow.hpp"

#include <glad/glad.h>
#include <engine/EngineException.hpp>
#include <engine/text/FontLoader.hpp>

#include <iostream>

void FluidSolverWindow::render() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, GetWidth(), GetHeight());

    std::string testing = "Testing";
    textRenderer->Render(testing, 24.0f, glm::vec2(100, 100), glm::vec4(1.0f));
    rectangleRenderer->RenderRectangle(glm::vec2(100, 100), glm::vec2(100, 100), glm::vec4(1.0f, 1.0f, 0.0f, 0.5f), glm::vec4(100.0f,100.0f, 50, 100));
}

FluidSolverWindow::FluidSolverWindow(const std::string &title, int width, int height) : Window(title, width, height) {}

bool FluidSolverWindow::even(int input) {
    if (input < 0)
        throw Engine::EngineException("negative not allowed");
    return input % 2 == 0;
}

void FluidSolverWindow::load() {
    loadFont();
    rectangleRenderer = new Engine::RectangleRenderer();
    rectangleRenderer->CreateProjectionMatrixForScreen(GetWidth(), GetHeight());
}

void FluidSolverWindow::loadFont() {
    Engine::Text::FontLoader loader("../resources/arialAscii.fnt");

    font = loader.loadToFont();
    textRenderer = new Engine::Text::TextRenderer(font);
    textRenderer->CreateProjectionMatrixForScreen(GetWidth(), GetHeight());

}