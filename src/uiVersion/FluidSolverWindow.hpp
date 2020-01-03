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
#include "ContinousVisualizerOpenGL.hpp"


#include <engine/graphics/Framebuffer.hpp>
#include <core/Simulation.hpp>
#include <dependencies/cppgui/library/src/basic/UIRunner.hpp>

#include <core/basicScenarios/Scenario.hpp>
#include <core/basicScenarios/HugeDamScenario.hpp>
#include <core/basicScenarios/SimpleBoxScenario.hpp>
#include <core/basicScenarios/MultipleHoleScenario.hpp>
#include <core/basicScenarios/BoxWithHoleScenario.hpp>
#include <core/basicScenarios/SimpleBoxScenarioSmallerParticles.hpp>
#include <core/basicScenarios/SimpleDamScenario.hpp>
#include <core/basicScenarios/BoundaryTestScenario.hpp>
#include <core/basicScenarios/SimpleBoxScenarioLargerParticles.hpp>

namespace FluidUI {
    class RootElement;
}


class FluidSolverWindow : public Engine::Window {

public:
    FluidSolverWindow(const std::string &title, int width = 800, int height = 600);

    void resetData();

    ~FluidSolverWindow() override;

    bool Pause = true;

    float RealTimeSpeed = 1.0f;

    /*bool saveFrames = false;
    float saveFramesPerSecond = 60.0f;
    float currentSaveFrameTime = 1.0f / saveFramesPerSecond;
    std::string imagePath = "./images/";
    unsigned int imageCounter = 0;*/

    std::vector<FluidSolver::Scenario *> Scenarios = std::vector<FluidSolver::Scenario *>({
                                                                                                  new FluidSolver::SimpleBoxScenario(),
                                                                                                  new FluidSolver::BoxWithHoleScenario(),
                                                                                                  new FluidSolver::MultipleHoleScenario(),
                                                                                                  new FluidSolver::SimpleBoxScenarioSmallerParticles(),
                                                                                                  new FluidSolver::SimpleBoxScenarioLargerParticles(),
                                                                                                  new FluidSolver::SimpleDamScenario(),
                                                                                                  new FluidSolver::HugeDamScenario(),
                                                                                                  new FluidSolver::BoundaryTestScenario(),
                                                                                          }

    );

    void SetScenario(FluidSolver::Scenario *scenario);

    FluidSolver::Scenario *GetScenario();

    FluidSolver::ITimestep* GetTimestep();

    void SetTimestep(FluidSolver::ITimestep* timestep);

    float GetRestDensity();

    void SetRestDensity(float restdensity);

    FluidSolver::IFluidSolver *GetFluidSolver();

    void SetFluidSolver(FluidSolver::IFluidSolver *solver);

private:

    FluidSolver::Simulation *simulation = nullptr;

    void setupSimulation();

    void onClick(float x, float y);


    ParticleVertexArray *particleVertexArray = nullptr;
    ParticleRenderer *particleRenderer = nullptr;
    ContinousVisualizerOpenGL* visualizerOpenGl = nullptr;


    void UpdateProjectionMatrices();

    void UpdateRectangleRendererProjectionMatrix();

    void UpdateParticleRendererProjectionMatrix(float particlesX, float particlesY, float particleSize);


    float accumulatedSimulationTime = 0.0f;

    FluidSolver::Scenario *scenario = nullptr;

    glm::vec4 particleFBORect = glm::vec4(0.0f);

    // rendering
    Engine::Graphics::Framebuffer *framebuffer = nullptr;
    uint32_t particleFBOWidth = 1920 * 2;
    uint32_t particleFBOHeight = 1080 * 2;
    Engine::Graphics::Texture2D *fboDepthTex = nullptr;
    Engine::Graphics::Texture2D *fboColorTex = nullptr;
    Engine::RectangleRenderer *rectangleRenderer = nullptr;

    void setupFBO();

    void saveAsImage();

    void setupUI();

    GuiEngineInterface *guiInterface = nullptr;
    cppgui::UIRunner *uiRunner = nullptr;
    Engine::Text::Font *font = nullptr;
    Engine::Text::Font *iconFont = nullptr;
    FluidUI::RootElement *entryPoint = nullptr;

protected:
    void render() override;

    void load() override;


};


#endif //FLUIDSOLVER_FLUIDSOLVERWINDOW_HPP
