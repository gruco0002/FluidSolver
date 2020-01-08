//
// Created by corbi on 17.04.2019.
//

#ifndef FLUIDSOLVER_FLUIDSOLVERWINDOW_HPP
#define FLUIDSOLVER_FLUIDSOLVERWINDOW_HPP


#include <engine/Window.hpp>

#include <engine/RectangleRenderer.hpp>

#include "ParticleVertexArray.hpp"
#include "ParticleRenderer.hpp"


#include <core/fluidSolver/SPHFluidSolver.hpp>
#include "DataLogger.hpp"
#include "ContinousVisualizerOpenGL.hpp"


#include <engine/graphics/Framebuffer.hpp>
#include <core/Simulation.hpp>

#include <core/basicScenarios/Scenario.hpp>
#include <core/basicScenarios/HugeDamScenario.hpp>
#include <core/basicScenarios/SimpleBoxScenario.hpp>
#include <core/basicScenarios/MultipleHoleScenario.hpp>
#include <core/basicScenarios/BoxWithHoleScenario.hpp>
#include <core/basicScenarios/SimpleBoxScenarioSmallerParticles.hpp>
#include <core/basicScenarios/SimpleDamScenario.hpp>
#include <core/basicScenarios/BoundaryTestScenario.hpp>
#include <core/basicScenarios/SimpleBoxScenarioLargerParticles.hpp>
#include <uiVersion/userInterface/MainUi.hpp>


class FluidSolverWindow : public Engine::Window {

public:
    explicit FluidSolverWindow(const std::string &title, int width = 800, int height = 600);

    void resetData();

protected:
    void unload() override;

public:
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

    FluidSolver::ITimestep *GetTimestep();

    void SetTimestep(FluidSolver::ITimestep *timestep);

    float GetRestDensity();

    void SetRestDensity(float restdensity);

    FluidSolver::IFluidSolver *GetFluidSolver();

    void SetFluidSolver(FluidSolver::IFluidSolver *solver);

    FluidSolver::ISimulationVisualizer *GetVisualizer();

    void SetVisualizer(FluidSolver::ISimulationVisualizer *visualizer);

    FluidSolver::StatisticCollector *GetStatisticCollector();

    FluidSolver::IParticleSelection *GetParticleSelection();

    void SetParticleSelection(FluidSolver::IParticleSelection *particleSelection);

private:

    FluidSolver::Simulation *simulation = nullptr;

    void setupSimulation();

    void onClick(float x, float y, Engine::Window::MouseButton button);


    void UpdateVisualizerViewport();


    float accumulatedSimulationTime = 0.0f;

    FluidSolver::Scenario *scenario = nullptr;


    size_t visualizerRenderTargetWidth = 1920;
public:
    size_t getVisualizerRenderTargetWidth() const;

    void setVisualizerRenderTargetWidth(size_t visualizerRenderTargetWidth);

    size_t getVisualizerRenderTargetHeight() const;

    void setVisualizerRenderTargetHeight(size_t visualizerRenderTargetHeight);

private:
    size_t visualizerRenderTargetHeight = 1080;

    void saveAsImage();

    void setupUI();

    FluidUI::MainUi *mainUi = nullptr;

    bool mouseInsideVisualization = false;
    glm::vec4 visualizationWindowCoordinates = glm::vec4(0.0f);

    enum SelectionType {
        SelectionTypeSelect,
        SelectionTypeDeselect
    };

    void SelectParticle(glm::vec2 position, SelectionType type);


protected:
    void render() override;

    void load() override;


};


#endif //FLUIDSOLVER_FLUIDSOLVERWINDOW_HPP
