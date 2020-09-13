#ifndef FLUIDSOLVER_FLUIDSOLVERWINDOW_HPP
#define FLUIDSOLVER_FLUIDSOLVERWINDOW_HPP


#include <engine/Window.hpp>

#include <engine/RectangleRenderer.hpp>

#include "ParticleVertexArray.hpp"
#include "ParticleRenderer.hpp"


// #include <core/fluidSolver/SESPHFluidSolver.hpp>
// #include "core/statistics/DataLogger.hpp"
// #include "ContinousVisualizerOpenGL.hpp"


#include <engine/graphics/Framebuffer.hpp>
#include <core/Simulation.hpp>

#include <core/scenario/Scenario.hpp>
//#include <core/basicScenarios/HugeDamScenario.hpp>
//#include <core/basicScenarios/SimpleBoxScenario.hpp>
//#include <core/basicScenarios/MultipleHoleScenario.hpp>
//#include <core/basicScenarios/BoxWithHoleScenario.hpp>
//#include <core/basicScenarios/SimpleBoxScenarioSmallerParticles.hpp>
//#include <core/basicScenarios/SimpleDamScenario.hpp>
//#include <core/basicScenarios/BoundaryTestScenario.hpp>
//#include <core/basicScenarios/SimpleBoxScenarioLargerParticles.hpp>
#include <uiVersion/userInterface/MainUi.hpp>
#include <core/fluidSolver/IISPHFluidSolver.hpp>


class FluidSolverWindow : public Engine::Window {

public:
    explicit FluidSolverWindow(const std::string &title, int width = 800, int height = 600);

    void resetData();

protected:
    void unload() override;

public:
    bool Pause = true;

    float RealTimeSpeed = 1.0f;

    bool saveFrames = false;
    float saveFramesForSecondsInitalValue = 30.0f;
    float saveFramesForSeconds = 30.0f;
    std::string imageFilepath = "./image.png";
    unsigned int imageCounter = 0;

    void stopRecordingAndresetImageSettings();

    void startRecording();


    std::vector<FluidSolver::Scenario *> Scenarios = std::vector<FluidSolver::Scenario *>({
                                                                                                  new FluidSolver::Scenario("../scenarios/test.chai"),
                                                                                          }

    );

    void SetScenario(FluidSolver::Scenario *scenario);

    FluidSolver::Scenario *GetScenario();

    FluidSolver::ITimestep *GetTimestep();

    void SetTimestep(FluidSolver::ITimestep *timestep);

    float GetRestDensity();

    void SetRestDensity(float restdensity);

    FluidSolver::IFluidSolverBase *GetFluidSolver();

    void SetFluidSolver(FluidSolver::IFluidSolverBase *solver);

    FluidSolver::ISimulationVisualizer *GetVisualizer();

    void SetVisualizer(FluidSolver::ISimulationVisualizer *visualizer);

    //FluidSolver::StatisticCollector *GetStatisticCollector();

    //FluidSolver::IParticleSelection *GetParticleSelection();

    //void SetParticleSelection(FluidSolver::IParticleSelection *particleSelection);

    //DataLogger *GetDataLogger();

    //void SetDataLogger(DataLogger *dataLogger);

    FluidSolver::Simulation* GetSimulation();

private:

    FluidSolver::Simulation *simulation = nullptr;

    void setupSimulation();

    void onMouseUp(float x, float y, Engine::Window::MouseButton button);

    void onMouseDown(float x, float y, Engine::Window::MouseButton button);

    void onMouseMove(float x, float y);


    void UpdateVisualizerViewport();


    float accumulatedSimulationTime = 0.0f;

    FluidSolver::Scenario *scenario = nullptr;


    size_t visualizerRenderTargetWidth = 1920;
public:
    size_t getVisualizerRenderTargetWidth() const;

    void setVisualizerRenderTargetWidth(size_t visualizerRenderTargetWidth);

    size_t getVisualizerRenderTargetHeight() const;

    void setVisualizerRenderTargetHeight(size_t visualizerRenderTargetHeight);

    std::vector<std::string> ErrorLog;

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

    void SelectParticles(glm::vec2 position1, glm::vec2 position2, SelectionType type);

    // mouse dragging stuff
    bool mouseDragStarted = false;
    bool isMouseDown = false;
    glm::vec2 mouseDragStart = glm::vec2(0.0f);
    glm::vec2 positionOnImageInSimSpaceDragStart = glm::vec2(0.0f);

protected:
    void render() override;

    void load() override;


};


#endif //FLUIDSOLVER_FLUIDSOLVERWINDOW_HPP
