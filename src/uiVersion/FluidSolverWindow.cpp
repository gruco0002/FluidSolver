#include "FluidSolverWindow.hpp"
#include "FluidSolverWindow.hpp"
#include "FluidSolverWindow.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <core/fluidSolver/kernel/CubicSplineKernel.hpp>
#include <core/fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch.hpp>
#include <core/fluidSolver/IISPHFluidSolver.hpp>
#include <thread>
#include <chrono>
#include <core/timestep/ConstantTimestep.hpp>
#include <imguiHelper.hpp>
#include <core/statistics/CachedStatisticCollector.hpp>
#include <core/selection/ParticleSelection.hpp>


void FluidSolverWindow::render() {

    if (this->Pause) {
        // sleep some time to save resources
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }


    // particle simulation
    bool simulationStepHappened = false;
    if (simulation != nullptr) {
        if (!this->Pause) {
            accumulatedSimulationTime += GetLastFrameTime() * RealTimeSpeed;

            while (accumulatedSimulationTime >= simulation->getTimestep()->getCurrentTimestep()) {
                //accumulatedSimulationTime -= sphFluidSolver->TimeStep;
                accumulatedSimulationTime = 0.0f; // we always want to render after a simulation step
                simulation->ExecuteSimulationStep();
                simulation->CollectStatistics();
                simulationStepHappened = true;
            }
        } else {
            accumulatedSimulationTime = 0.0f;
        }
    }


    // visualize the simulation
    if (simulation != nullptr) {
        simulation->VisualizeSimulation();
    }

    // force pending opengl stuff to finish
    glFlush();

    // save as image
    if (simulationStepHappened)
        saveAsImage();

    // render to screen
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, GetFramebufferWidth(), GetFramebufferHeight());

    ImGuiHelper::PreRender();


    auto id = ImGui::DockSpaceOverViewport();
    // render your GUI


    ImGui::Begin("Simulation Visualization");

    if (simulation != nullptr) {
        auto glRenderer = dynamic_cast<IOpenGLVisualizer *>(simulation->getSimulationVisualizer());
        if (glRenderer != nullptr) {
            auto tex = glRenderer->GetTexture();
            // render visualization
            auto maxRegion = ImGui::GetContentRegionMax();
            maxRegion.x -= 20.0f;
            maxRegion.y -= 30.0f;
            float width = 0.0f;
            float height = 0.0f;
            if ((float) tex->getWidth() / (float) tex->getHeight() * maxRegion.y > maxRegion.x) {
                // height is too small
                width = maxRegion.x;
                height = (float) tex->getHeight() / (float) tex->getWidth() * maxRegion.x;
            } else {
                height = maxRegion.y;
                width = (float) tex->getWidth() / (float) tex->getHeight() * maxRegion.y;
            }


            // im gui image location finder trick
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

            if (ImGui::BeginChild("SimView")) {

                bool imageHasFocus = ImGui::IsWindowHovered();
                auto pos = ImGui::GetWindowPos();
                auto padding = ImGui::GetStyle().WindowPadding;
                auto windowPosition = glm::vec2(pos.x + padding.x, pos.y + padding.y);
                auto windowSize = glm::vec2(width, height);
                visualizationWindowCoordinates = glm::vec4(windowPosition.x, windowPosition.y, windowSize.x,
                                                           windowSize.y);
                mouseInsideVisualization = imageHasFocus;

                ImGui::PushItemWidth(width);

                ImGui::Image((void *) tex->GetID(), ImVec2(width, height));


                if (mouseDragStarted) {
                    // draw selection rectangle
                    glm::vec2 currentMousePos = glm::vec2((float) this->GetMousePositionX(),
                                                          (float) this->GetMousePositionY());
                    ImDrawList *drawList = ImGui::GetWindowDrawList();
                    drawList->AddRect(ImVec2(mouseDragStart.x, mouseDragStart.y),
                                      ImVec2(currentMousePos.x, currentMousePos.y), 0xFF0000FF);
                }

                ImGui::PopItemWidth();

            }

            ImGui::PopStyleVar(3);

            ImGui::EndChild();

        } else {
            ImGui::Text("No OpenGL compatible visualizer!");
        }
    }

    ImGui::End();

    mainUi->Run();

    ImGuiHelper::PostRender();

}

FluidSolverWindow::FluidSolverWindow(const std::string &title, int width, int height) : Window(title, width, height) {


}


void FluidSolverWindow::load() {

    OnKeyPressed.Subscribe([=](int keyCode) { if (keyCode == GLFW_KEY_SPACE)this->Pause = !this->Pause; });
    OnMouseDown.Subscribe([=](Engine::Window::MouseButton btn) {
        this->onMouseDown(this->GetMousePositionX(), this->GetMousePositionY(), btn);
    });
    OnMouseUp.Subscribe([=](Engine::Window::MouseButton btn) {
        this->onMouseUp(this->GetMousePositionX(), this->GetMousePositionY(), btn);
    });
    OnCursorPositionChanged.Subscribe([=](double x, double y) {
        this->onMouseMove(this->GetMousePositionX(), this->GetMousePositionY());
    });

    setupSimulation();
    setupUI();

    // SETUP IMGUI
    ImGuiHelper::Init(this->GetWindowHandler());

}

void FluidSolverWindow::setupSimulation() {

    this->scenario = new FluidSolver::SimpleBoxScenario();


    simulation = new FluidSolver::Simulation();

    // set particle size and timestep
    simulation->setTimestep(new FluidSolver::ConstantTimestep(0.001f));
    simulation->setParticleSize(scenario->GetParticleSize());
    simulation->setRestDensity(1.0f);
    simulation->setGravity(9.81f);

    // set up basic stuff
    /*auto sphFluidSolver = new FluidSolver::SPHFluidSolver();
    sphFluidSolver->StiffnessK = 100000.0f;
    sphFluidSolver->Viscosity = 5.0f;
    simulation->setFluidSolver(sphFluidSolver);*/


    auto isphFluidSolver = new FluidSolver::IISPHFluidSolver();
    isphFluidSolver->Omega = 0.5f;
    isphFluidSolver->Gamma = 0.7f;
    isphFluidSolver->MaxDensityErrorAllowed = 0.001f;
    simulation->setFluidSolver(isphFluidSolver);


    // set up scenario data
    auto particleCollection = scenario->GenerateScenario(simulation->getRestDensity());
    simulation->setParticleCollection(particleCollection);

    // add simulation modifiers
    simulation->clearSimulationModifiers();
    for (FluidSolver::ISimulationModifier *mod : scenario->GetSimulationModifiers()) {
        simulation->addSimulationModifier(mod);
    }

    // create particle renderer
    auto particleRenderer = new ParticleRenderer();
    simulation->setSimulationVisualizer(particleRenderer);


    UpdateVisualizerViewport();


    // reset simulation time
    accumulatedSimulationTime = 0.0f;

    // set statistics collection
    simulation->setStatisticCollector(new FluidSolver::CachedStatisticCollector());

    // set datalogger but don't start it
    simulation->setDataLogger(new DataLogger("data.csv"));

}


void FluidSolverWindow::resetData() {

    if (simulation->getDataLogger() != nullptr) {
        auto logger = simulation->getDataLogger();
        logger->FinishLogging();
        logger->ResetLogger();
    }

    // set particle size
    simulation->setParticleSize(scenario->GetParticleSize());


    // set up values
    auto fluidSolver = dynamic_cast<FluidSolver::SPHFluidSolver *>(simulation->getFluidSolver());
    if (fluidSolver != nullptr) {

    }
    auto ifluidSolver = dynamic_cast<FluidSolver::IISPHFluidSolver *>(simulation->getFluidSolver());
    if (ifluidSolver != nullptr) {

    }

    // set up scenario data, delete old particle collection
    delete simulation->getParticleCollection();
    auto particleCollection = scenario->GenerateScenario(simulation->getRestDensity());
    simulation->setParticleCollection(particleCollection);

    // set up simulation modifiers
    simulation->clearSimulationModifiers();
    for (FluidSolver::ISimulationModifier *mod : scenario->GetSimulationModifiers()) {
        simulation->addSimulationModifier(mod);
    }


    UpdateVisualizerViewport();


    /* this->imageCounter = 0;
     this->currentSaveFrameTime = 1.0f / this->saveFramesPerSecond;*/

}

void FluidSolverWindow::onMouseDown(float x, float y, Engine::Window::MouseButton button) {
    if (simulation == nullptr) return;
    if (simulation->getSimulationVisualizer() == nullptr) return;
    if (!mouseInsideVisualization)return;

    isMouseDown = true;


    // setting coordinates for the drag start
    mouseDragStart = glm::vec2(x, y);
    mouseDragStarted = false;

    // calculating position on image on drag start in simulation space
    glm::vec2 positionOnImage = glm::vec2(x - visualizationWindowCoordinates.x, y - visualizationWindowCoordinates.y);
    positionOnImage.x = positionOnImage.x / visualizationWindowCoordinates.z;
    positionOnImage.y = positionOnImage.y / visualizationWindowCoordinates.w;
    positionOnImage.x = positionOnImage.x * (float) visualizerRenderTargetWidth;
    positionOnImage.y = positionOnImage.y * (float) visualizerRenderTargetHeight;
    positionOnImageInSimSpaceDragStart = simulation->getSimulationVisualizer()->ConvertPixelCoordinateToParticleSpace(
            (size_t) positionOnImage.x,
            (size_t) positionOnImage.y);

}

void FluidSolverWindow::onMouseMove(float x, float y) {
    const float DRAG_START_THRESHOLD_DISTANCE = 3.5f;
    if (isMouseDown) {
        glm::vec2 tmp = glm::vec2(x, y);
        if (glm::length(tmp - mouseDragStart) >= DRAG_START_THRESHOLD_DISTANCE) {
            mouseDragStarted = true;
        }
    }
}


void FluidSolverWindow::onMouseUp(float x, float y, Engine::Window::MouseButton button) {
    isMouseDown = false;

    if (simulation == nullptr) return;
    if (simulation->getSimulationVisualizer() == nullptr) return;
    if (!mouseInsideVisualization)return;


    glm::vec2 positionOnImage = glm::vec2(x - visualizationWindowCoordinates.x, y - visualizationWindowCoordinates.y);
    positionOnImage.x = positionOnImage.x / visualizationWindowCoordinates.z;
    positionOnImage.y = positionOnImage.y / visualizationWindowCoordinates.w;
    positionOnImage.x = positionOnImage.x * (float) visualizerRenderTargetWidth;
    positionOnImage.y = positionOnImage.y * (float) visualizerRenderTargetHeight;


    auto pos = simulation->getSimulationVisualizer()->ConvertPixelCoordinateToParticleSpace((size_t) positionOnImage.x,
                                                                                            (size_t) positionOnImage.y);

    auto selectionType = button == Engine::Window::MouseButton::RightButton ? SelectionType::SelectionTypeDeselect
                                                                            : SelectionTypeSelect;
    if (mouseDragStarted) {
        mouseDragStarted = false;
        this->SelectParticles(pos, positionOnImageInSimSpaceDragStart, selectionType);
    } else {
        this->SelectParticle(pos, selectionType);
    }

}


void FluidSolverWindow::saveAsImage() {
    if (!saveFrames)
        return;

    if (saveFramesForSeconds <= 0.0f) {
        stopRecordingAndresetImageSettings();
        return;
    }

    if (simulation == nullptr)
        return;
    auto glRenderer = dynamic_cast<IOpenGLVisualizer *>(simulation->getSimulationVisualizer());
    if (glRenderer == nullptr)
        return;

    auto tex = glRenderer->GetTexture();
    if (tex == nullptr)
        return;

    if (simulation->getTimestep() == nullptr)
        return;

    float timestep = simulation->getTimestep()->getCurrentTimestep();
    saveFramesForSeconds -= timestep;

    std::string newFilepath =
            imageFilepath.substr(0, imageFilepath.length() - 4) + std::to_string(imageCounter) + ".png";
    tex->SaveAsPNG(newFilepath);

    imageCounter++;

}

void FluidSolverWindow::setupUI() {

    mainUi = new FluidUI::MainUi(this);

}

void FluidSolverWindow::SetScenario(FluidSolver::Scenario *scenario) {
    if (this->scenario == scenario)
        return;
    this->scenario = scenario;
    resetData();
    UpdateVisualizerViewport();
}

FluidSolver::Scenario *FluidSolverWindow::GetScenario() {
    return scenario;
}

FluidSolver::ITimestep *FluidSolverWindow::GetTimestep() {
    return simulation->getTimestep();
}

void FluidSolverWindow::SetTimestep(FluidSolver::ITimestep *timestep) {
    simulation->setTimestep(timestep);
}

float FluidSolverWindow::GetRestDensity() {
    return simulation->getRestDensity();
}

void FluidSolverWindow::SetRestDensity(float restdensity) {
    simulation->setRestDensity(restdensity);
}

FluidSolver::IFluidSolver *FluidSolverWindow::GetFluidSolver() {
    return simulation->getFluidSolver();
}

void FluidSolverWindow::SetFluidSolver(FluidSolver::IFluidSolver *solver) {
    simulation->setFluidSolver(solver);
}


FluidSolver::ISimulationVisualizer *FluidSolverWindow::GetVisualizer() {
    return simulation->getSimulationVisualizer();
}

void FluidSolverWindow::UpdateVisualizerViewport() {
    if (simulation == nullptr)
        return;
    auto vis = simulation->getSimulationVisualizer();
    if (vis == nullptr)
        return;
    vis->setRenderTargetSize(this->visualizerRenderTargetWidth, this->visualizerRenderTargetHeight);
    if (scenario == nullptr)
        return;

    int particlesX = scenario->GetParticleCountX();
    int particlesY = scenario->GetParticleCountY();

    float width = (float) particlesX;
    float height = (float) particlesY;
    FluidSolver::ISimulationVisualizer::SimulationViewArea viewArea;
    viewArea.Top = height / 2.0f;
    viewArea.Bottom = -height / 2.0f;
    viewArea.Left = -width / 2.0f;
    viewArea.Right = width / 2.0f;

    vis->setSimulationViewArea(viewArea);

}

size_t FluidSolverWindow::getVisualizerRenderTargetWidth() const {
    return visualizerRenderTargetWidth;
}

void FluidSolverWindow::setVisualizerRenderTargetWidth(size_t visualizerRenderTargetWidth) {
    if (this->visualizerRenderTargetWidth != visualizerRenderTargetWidth) {
        this->visualizerRenderTargetWidth = visualizerRenderTargetWidth;
        UpdateVisualizerViewport();
    }
}

size_t FluidSolverWindow::getVisualizerRenderTargetHeight() const {
    return visualizerRenderTargetHeight;
}

void FluidSolverWindow::setVisualizerRenderTargetHeight(size_t visualizerRenderTargetHeight) {
    if (this->visualizerRenderTargetHeight != visualizerRenderTargetHeight) {
        this->visualizerRenderTargetHeight = visualizerRenderTargetHeight;
        UpdateVisualizerViewport();
    }
}

void FluidSolverWindow::SetVisualizer(FluidSolver::ISimulationVisualizer *visualizer) {
    this->simulation->setSimulationVisualizer(visualizer);
    UpdateVisualizerViewport();
}

FluidSolver::StatisticCollector *FluidSolverWindow::GetStatisticCollector() {
    return simulation->getStatisticCollector();
}

void FluidSolverWindow::unload() {
    delete mainUi;
    ImGuiHelper::Uninit();
}

void FluidSolverWindow::SetParticleSelection(FluidSolver::IParticleSelection *particleSelection) {
    simulation->setParticleSelection(particleSelection);
}

FluidSolver::IParticleSelection *FluidSolverWindow::GetParticleSelection() {
    return simulation->getParticleSelection();
}

void FluidSolverWindow::SelectParticle(glm::vec2 position, FluidSolverWindow::SelectionType type) {

    auto custom = dynamic_cast<FluidSolver::ParticleSelection *>(simulation->getParticleSelection());

    // find nearest particle, that you have clicked on depending on type
    uint32_t particleIndex = -1;
    float dist = simulation->getParticleSize();
    for (uint32_t i = 0; i < simulation->getParticleCollection()->GetSize(); i++) {
        auto particleType = simulation->getParticleCollection()->GetParticleType(i);
        if (particleType == FluidSolver::ParticleTypeDead)
            continue;

        if (custom != nullptr) {
            auto isSelected = custom->IsParticleSelected(i, simulation->getParticleCollection());
            if (type == SelectionType::SelectionTypeSelect && isSelected) {
                continue; // skip this one, since it is already selected
            } else if (type == SelectionType::SelectionTypeDeselect && !isSelected) {
                continue; // skip this one, since it is already deselected
            }
        }
        auto particlePos = simulation->getParticleCollection()->GetPosition(i);
        auto partDist = glm::length(position - particlePos);
        if (partDist < dist) {
            particleIndex = i;
            dist = partDist;
        }
    }

    // select corresponding particle(s)
    if (particleIndex != -1) {
        if (custom == nullptr) {
            custom = new FluidSolver::ParticleSelection();
            delete simulation->getParticleSelection();
            simulation->setParticleSelection(custom);
        }
        if (type == SelectionType::SelectionTypeSelect) {
            custom->AddParticleToSelection(particleIndex);
        } else if (type == SelectionType::SelectionTypeDeselect) {
            custom->RemoveParticleFromSelection(particleIndex);
        }
    }
}

void
FluidSolverWindow::SelectParticles(glm::vec2 position1, glm::vec2 position2, FluidSolverWindow::SelectionType type) {

    float minX = std::fmin(position1.x, position2.x);
    float minY = std::fmin(position1.y, position2.y);
    float maxX = std::fmax(position1.x, position2.x);
    float maxY = std::fmax(position1.y, position2.y);

    // find nearest particle, that you have clicked on depending on type
    std::vector<uint32_t> particles;
    for (uint32_t i = 0; i < simulation->getParticleCollection()->GetSize(); i++) {
        auto particleType = simulation->getParticleCollection()->GetParticleType(i);
        if (particleType == FluidSolver::ParticleTypeDead)
            continue;


        auto particlePos = simulation->getParticleCollection()->GetPosition(i);
        if (particlePos.x >= minX && particlePos.x <= maxX) {
            if (particlePos.y >= minY && particlePos.y <= maxY) {
                particles.push_back(i);
            }
        }
    }

    // select corresponding particle(s)
    auto custom = dynamic_cast<FluidSolver::ParticleSelection *>(simulation->getParticleSelection());

    for (auto particleIndex:particles) {
        if (custom == nullptr) {
            custom = new FluidSolver::ParticleSelection();
            delete simulation->getParticleSelection();
            simulation->setParticleSelection(custom);
        }
        if (type == SelectionType::SelectionTypeSelect) {
            custom->AddParticleToSelection(particleIndex);
        } else if (type == SelectionType::SelectionTypeDeselect) {
            custom->RemoveParticleFromSelection(particleIndex);
        }
    }


}

DataLogger *FluidSolverWindow::GetDataLogger() {
    return simulation->getDataLogger();
}

void FluidSolverWindow::SetDataLogger(DataLogger *dataLogger) {
    simulation->setDataLogger(dataLogger);
}

void FluidSolverWindow::stopRecordingAndresetImageSettings() {
    saveFrames = false;
    saveFramesForSeconds = saveFramesForSecondsInitalValue;
    imageCounter = 0;
}

void FluidSolverWindow::startRecording() {
    saveFramesForSecondsInitalValue = saveFramesForSeconds;
    saveFrames = true;
    imageCounter = 0;
}



