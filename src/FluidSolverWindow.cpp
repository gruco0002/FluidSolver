//
// Created by corbi on 17.04.2019.
//

#include "FluidSolverWindow.hpp"
#include "FluidSolverTopMenu.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <engine/EngineException.hpp>
#include <engine/text/FontLoader.hpp>

#include <iostream>
#include <dependencies/cppgui/src/Theme.hpp>
#include <dependencies/cppgui/src/AlignBox.hpp>
#include <dependencies/cppgui/src/Spread.hpp>


void FluidSolverWindow::render() {
    fpsLabel->setText("FPS: " + std::to_string(GetFPS()));
    particleVertexArray->Update();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, GetFramebufferWidth(), GetFramebufferHeight());

    particleRenderer->Render();
    uiWrapper->render();
}

FluidSolverWindow::FluidSolverWindow(const std::string &title, int width, int height) : Window(title, width, height) {}

bool FluidSolverWindow::even(int input) {
    if (input < 0)
        throw Engine::EngineException("negative not allowed");
    return input % 2 == 0;
}

void FluidSolverWindow::load() {
    loadParticles();
    loadFont();
    loadGUI();
}

void FluidSolverWindow::loadFont() {
    Engine::Text::FontLoader loader("../resources/arialAscii.fnt");

    font = loader.loadToFont();

}

void FluidSolverWindow::loadGUI() {
    guiInterface = new GuiEngineInterface(this, font);
    uiWrapper = new cppgui::DynamicUIWrapper(guiInterface);

    OnWindowSizeChanged.Subscribe([=](int width, int height) {
        uiWrapper->renderDimensionsUpdated();
    });
    OnCursorPositionChanged.Subscribe([=](double xPos, double yPos) {
        uiWrapper->MousePositionInput(cppgui::Vector2(xPos, yPos));
    });
    OnMouseDown.Subscribe([=](MouseButton btn) {
        uiWrapper->MouseDown(cppgui::Vector2(GetMousePositionX(), GetMousePositionY()));
    });
    OnMouseUp.Subscribe([=](MouseButton btn) {
        uiWrapper->MouseUp(cppgui::Vector2(GetMousePositionX(), GetMousePositionY()));
    });
    OnScrollChanged.Subscribe([=](double xOffset, double yOffset) {
        uiWrapper->MouseWheel(xOffset, yOffset);
    });
    OnKeyPressed.Subscribe([=](int keycode) {
        if (keycode == GLFW_KEY_BACKSPACE)
            uiWrapper->KeyDown(cppgui::KeyBackspace, false, "");
    });
    OnKeyRelease.Subscribe([=](int keycode) {
        if (keycode == GLFW_KEY_BACKSPACE)
            uiWrapper->KeyUp(cppgui::KeyBackspace, false, "");
    });
    OnTextInput.Subscribe([=](std::string text) {
        uiWrapper->KeyDown(cppgui::KeyNone, true, text);
    });
    buildGUI();
}

void FluidSolverWindow::buildGUI() {
    auto theme = cppgui::Theme::getIndigoPink();
    auto scaff = new cppgui::Scaffold(0, 0, 0, 0, theme);
    uiWrapper->setScaffold(scaff);

    auto alignTop = new cppgui::AlignBox(cppgui::AlignmentTop, 50.0f);
    alignTop->addChild(new cppgui::Spread(new FluidSolverTopMenu(particleRenderer)));

    scaff->addChild(alignTop);

    fpsLabel = new cppgui::Label( 0,55, "FPS: ");
    scaff->addChild(fpsLabel);
    fpsLabel->Visible = true;

}

void FluidSolverWindow::loadParticles() {

    // generate 1 mio particles
    std::vector<FluidSolver::SimpleParticleCollection::FluidParticle> particles(1000 * 1000);
    for (int x = 0; x < 1000; x++) {
        for (int y = 0; y < 1000; y++) {
            FluidSolver::SimpleParticleCollection::FluidParticle p;
            p.Position = glm::vec2((float) x, (float) y);
            p.Velocity = glm::vec2((float) x / 1000.0f, (float) y / 1000.0f);
            p.Acceleration = glm::vec2((float) (1000.0f - x) / 1000.0f, (float) y / 1000.0f);
            p.Pressure = 0.5f;
            p.Density = 10000.0f;
            p.Mass = -4.0f;

            particles[x * 1000 + y] = p;
        }
    }

    auto simple = new FluidSolver::SimpleParticleCollection(particles);
    particleVertexArray = new ParticleVertexArray(simple);

    particleRenderer = new ParticleRenderer(particleVertexArray, ParticleRenderer::GenerateOrtho(0, 1000, 0, 1000));

}
