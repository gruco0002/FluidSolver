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
#include <core/basicScenarios/BoxWithHoleScenario.hpp>
#include <core/basicScenarios/MultipleHoleScenario.hpp>
#include <engine/graphics/Framebuffer.hpp>


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

	void UpdateProjectionMatrices();

	void UpdateRectangleRendererProjectionMatrix();

	void UpdateParticleRendererProjectionMatrix(float particlesX, float particlesY, float particleSize);



    float accumulatedSimulationTime = 0.0f;

    FluidSolver::Scenario *scenario = new FluidSolver::MultipleHoleScenario();

    DataLogger *dataLogger = nullptr;

    // rendering
    Engine::Graphics::Framebuffer *framebuffer = nullptr;
    uint32_t framebufferWidth = 1920;
    uint32_t framebufferHeight = 1080;
    Engine::Graphics::Texture2D* fboDepthTex = nullptr;
    Engine::Graphics::Texture2D* fboColorTex = nullptr;
	Engine::RectangleRenderer* rectangleRenderer = nullptr;

    void setupFBO();

protected:
    void render() override;

    void load() override;


};


#endif //FLUIDSOLVER_FLUIDSOLVERWINDOW_HPP
