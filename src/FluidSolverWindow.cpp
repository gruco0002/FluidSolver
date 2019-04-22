//
// Created by corbi on 17.04.2019.
//

#include "FluidSolverWindow.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <engine/EngineException.hpp>
#include <engine/text/FontLoader.hpp>

#include <iostream>
#include <dependencies/cppgui/src/Theme.hpp>
#include <dependencies/cppgui/src/Button.hpp>
#include <dependencies/cppgui/src/TextInput.hpp>

void FluidSolverWindow::render() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, GetFramebufferWidth(), GetFramebufferHeight());

    uiWrapper->render();
}

FluidSolverWindow::FluidSolverWindow(const std::string &title, int width, int height) : Window(title, width, height) {}

bool FluidSolverWindow::even(int input) {
    if (input < 0)
        throw Engine::EngineException("negative not allowed");
    return input % 2 == 0;
}

void FluidSolverWindow::load() {
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

    OnFramebufferSizeChanged.Subscribe([=](int width, int height) {
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

    auto btn = new cppgui::Button("Testing", 100, 100, 250, 50);
    scaff->addChild(btn);

    auto ti = new cppgui::TextInput(100, 200, 250, 50, "Test");
    scaff->addChild(ti);

}
