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
#include <core/SPHFluidSolver.hpp>
#include "FluidSolverParticleInfoGUI.hpp"
#include "DataLogger.hpp"
#include <core/basicScenarios/Scenario.hpp>
#include <core/basicScenarios/SimpleBoxScenario.hpp>


class FluidSolverWindow : public Engine::Window {

public:
    FluidSolverWindow(const std::string &title, int width = 800, int height = 600);

    static bool even(int input);

    void resetData();

    FluidSolver::SPHFluidSolver *sphFluidSolver = nullptr;

    FluidSolver::IParticleCollection *particleCollection = nullptr;

    bool Pause = true;

    float RealTimeSpeed = 1.0f;

private:
    Engine::Text::Font *font;

    void loadFont();

    void loadGUI();

    void buildGUI();

    void loadParticles();

    void onClick(float x, float y);


    cppgui::DynamicUIWrapper *uiWrapper;
    GuiEngineInterface *guiInterface;

    ParticleVertexArray *particleVertexArray = nullptr;
    ParticleRenderer *particleRenderer = nullptr;


    cppgui::Label *fpsLabel;

    FluidSolverParticleInfoGUI *infoBox = nullptr;

    void CalculateCorrectProjectionMatrix(float particlesX, float particlesY, float particleSize);

    float accumulatedSimulationTime = 0.0f;

    FluidSolver::Scenario *scenario = new FluidSolver::SimpleBoxScenario();

    DataLogger* dataLogger = nullptr;

protected:
    void render() override;

    void load() override;


};


#endif //FLUIDSOLVER_FLUIDSOLVERWINDOW_HPP
