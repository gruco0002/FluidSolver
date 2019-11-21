//
// Created by corbi on 17.04.2019.
//

#ifndef FLUIDSOLVER_FLUIDSOLVERWINDOW_HPP
#define FLUIDSOLVER_FLUIDSOLVERWINDOW_HPP


#include <engine/Window.hpp>
#include <engine/text/Font.hpp>
#include <engine/text/TextRenderer.hpp>
#include <engine/RectangleRenderer.hpp>

#include "GuiEngineInterface.hpp"
#include "ParticleVertexArray.hpp"
#include "ParticleRenderer.hpp"


#include <core/fluidSolver/SPHFluidSolver.hpp>
#include "DataLogger.hpp"
#include <core/basicScenarios/Scenario.hpp>
#include <core/basicScenarios/BoxWithHoleScenario.hpp>
#include <core/basicScenarios/MultipleHoleScenario.hpp>
#include <engine/graphics/Framebuffer.hpp>
#include <core/Simulation.hpp>
#include <core/basicScenarios/SimpleDamScenario.hpp>
#include <core/basicScenarios/HugeDamScenario.hpp>
#include <core/basicScenarios/SimpleBoxScenario.hpp>


class FluidSolverWindow : public Engine::Window {

public:
    FluidSolverWindow(const std::string &title, int width = 800, int height = 600);

    static bool even(int input);

    void resetData();

    bool Pause = true;

    float RealTimeSpeed = 1.0f;

    /*bool saveFrames = false;
    float saveFramesPerSecond = 60.0f;
    float currentSaveFrameTime = 1.0f / saveFramesPerSecond;
    std::string imagePath = "./images/";
    unsigned int imageCounter = 0;*/

private:

    FluidSolver::Simulation* simulation = nullptr;

    void setupSimulation();

    void onClick(float x, float y);



    GuiEngineInterface *guiInterface = nullptr;

    ParticleVertexArray *particleVertexArray = nullptr;
    ParticleRenderer *particleRenderer = nullptr;



    void UpdateProjectionMatrices();

    void UpdateRectangleRendererProjectionMatrix();

    void UpdateParticleRendererProjectionMatrix(float particlesX, float particlesY, float particleSize);


    float accumulatedSimulationTime = 0.0f;

    FluidSolver::Scenario *scenario = new FluidSolver::SimpleBoxScenario();



    // rendering
    Engine::Graphics::Framebuffer *framebuffer = nullptr;
    uint32_t framebufferWidth = 1920 * 2;
    uint32_t framebufferHeight = 1080 * 2;
    Engine::Graphics::Texture2D *fboDepthTex = nullptr;
    Engine::Graphics::Texture2D *fboColorTex = nullptr;
    Engine::RectangleRenderer *rectangleRenderer = nullptr;

    void setupFBO();




    void saveAsImage();

protected:
    void render() override;

    void load() override;


};


#endif //FLUIDSOLVER_FLUIDSOLVERWINDOW_HPP