//
// Created by corbi on 17.04.2019.
//

#ifndef FLUIDSOLVER_FLUIDSOLVERWINDOW_HPP
#define FLUIDSOLVER_FLUIDSOLVERWINDOW_HPP


#include <engine/Window.hpp>
#include <engine/text/Font.hpp>
#include <engine/text/TextRenderer.hpp>
#include <engine/RectangleRenderer.hpp>
#include <dependencies/cppgui/src/DynamicUIWrapper.hpp>
#include "GuiEngineInterface.hpp"
#include "ParticleVertexArray.hpp"
#include "ParticleRenderer.hpp"

#include <dependencies/cppgui/src/Label.hpp>

class FluidSolverWindow : public Engine::Window {

public:
    FluidSolverWindow(const std::string &title, int width = 800, int height = 600);

    static bool even(int input);

private:
    Engine::Text::Font *font;

    void loadFont();

    void loadGUI();

    void buildGUI();

    void loadParticles();

    Engine::Text::TextRenderer *textRenderer;
    Engine::RectangleRenderer *rectangleRenderer;

    cppgui::DynamicUIWrapper *uiWrapper;
    GuiEngineInterface *guiInterface;

    ParticleVertexArray *particleVertexArray;
    ParticleRenderer *particleRenderer;

    cppgui::Label* fpsLabel;

protected:
    void render() override;

    void load() override;


};


#endif //FLUIDSOLVER_FLUIDSOLVERWINDOW_HPP
