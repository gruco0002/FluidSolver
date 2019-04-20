//
// Created by corbi on 17.04.2019.
//

#include "FluidSolverWindow.hpp"

#include <glad/glad.h>
#include <engine/EngineException.hpp>
#include <engine/text/FontLoader.hpp>
#include <libraries/glm/gtc/matrix_transform.hpp>

void FluidSolverWindow::render() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0,0, GetWidth(), GetHeight());

    std::string testing = "Testing";
    textRenderer->Render(testing, 240.0f, glm::vec2(100, 100), glm::vec4(1.0f));
}

FluidSolverWindow::FluidSolverWindow(const std::string &title, int width, int height) : Window(title, width, height) {}

bool FluidSolverWindow::even(int input) {
    if (input < 0)
        throw Engine::EngineException("negative not allowed");
    return input % 2 == 0;
}

void FluidSolverWindow::load() {
    loadFont();
}

void FluidSolverWindow::loadFont() {
    Engine::Text::FontLoader loader("../resources/arialAscii.fnt");

    font = loader.loadToFont();
    textRenderer = new Engine::Text::TextRenderer(font);
    textRenderer->projectionMatrix = glm::ortho(0, GetWidth(), 		GetHeight(), 0);
}
