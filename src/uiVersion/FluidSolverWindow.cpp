#include "FluidSolverWindow.hpp"
#include "FluidSolverWindow.hpp"
//
// Created by corbi on 17.04.2019.
//

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


    if (particleVertexArray != nullptr)
        particleVertexArray->Update();

    // render particles to fbo
    framebuffer->Bind(true);

    if (simulation != nullptr) {
        simulation->VisualizeSimulation();
    }

    framebuffer->Unbind();
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

    ImGui::SetNextWindowDockID(id);
    ImGui::Begin("Simulation Visualization");

    // render visualization
    auto maxRegion = ImGui::GetContentRegionMax();
    float width = 0.0f;
    float height = 0.0f;
    if ((float) fboColorTex->getWidth() / (float) fboColorTex->getHeight() * maxRegion.y > maxRegion.x) {
        // height is too small
        width = maxRegion.x;
        height = (float) fboColorTex->getHeight() / (float) fboColorTex->getWidth() * maxRegion.x;
    } else {
        height = maxRegion.y;
        width = (float) fboColorTex->getWidth() / (float) fboColorTex->getHeight() * maxRegion.y;
    }
    ImGui::Image((void *) fboColorTex->GetID(), ImVec2(width, height), ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();

    mainUi->Run();

    ImGuiHelper::PostRender();

}

FluidSolverWindow::FluidSolverWindow(const std::string &title, int width, int height) : Window(title, width, height) {


}


void FluidSolverWindow::load() {

    OnKeyPressed.Subscribe([=](int keyCode) { if (keyCode == GLFW_KEY_SPACE)this->Pause = !this->Pause; });
    OnMouseDown.Subscribe([=](Engine::Window::MouseButton btn) {
        this->onClick(this->GetMousePositionX(), this->GetMousePositionY());
    });
    OnFramebufferSizeChanged.Subscribe([=](int width, int height) { this->UpdateProjectionMatrices(); });
    setupFBO();
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
    particleRenderer = new ParticleRenderer(ParticleRenderer::GenerateOrtho(-10, 10, 10, -10));
    particleRenderer->pointSize = 30.0f;
    particleRenderer->colorSelection = ParticleRenderer::ColorSelection::Density;
    particleRenderer->topValue = 2.0f;
    particleRenderer->bottomValue = 0.0f;
    particleRenderer->topColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    particleRenderer->bottomColor = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
    simulation->setSimulationVisualizer(particleRenderer);

    /*visualizerOpenGl = new ContinousVisualizerOpenGL();
    visualizerOpenGl->SetScenarioSize(scenario);
    simulation->setSimulationVisualizer(visualizerOpenGl);*/

    this->UpdateProjectionMatrices();


    // reset simulation time
    accumulatedSimulationTime = 0.0f;

    // set statistics collection
    simulation->setStatisticCollector(new FluidSolver::StatisticCollector());


    // setup dataLogger
    auto dataLogger = new DataLogger("log.csv");
    simulation->setDataLogger(dataLogger);
    dataLogger->StartLogging();

}


void FluidSolverWindow::resetData() {
    if (simulation->getDataLogger())
        simulation->getDataLogger()->FinishLogging();

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


    this->UpdateProjectionMatrices();

    if (visualizerOpenGl != nullptr)
        visualizerOpenGl->SetScenarioSize(scenario);

    if (simulation->getDataLogger())
        simulation->getDataLogger()->StartLogging();

    /* this->imageCounter = 0;
     this->currentSaveFrameTime = 1.0f / this->saveFramesPerSecond;*/

}


void FluidSolverWindow::onClick(float x, float y) {
    return;
    // TODO: fix
    // pos is the position in particle space
    glm::vec2 pos = glm::vec2(0, 0);

    // find nearest particle, that you have clicked on
    uint32_t particleIndex = -1;
    float dist = particleRenderer->pointSize * 0.5;
    for (uint32_t i = 0; i < simulation->getParticleCollection()->GetSize(); i++) {
        auto particlePos = simulation->getParticleCollection()->GetPosition(i);
        auto partDist = glm::length(pos - particlePos);
        if (partDist < dist) {
            particleIndex = i;
            dist = partDist;
        }
    }

    // set particle index in info box
    if (particleIndex != -1) {

        particleRenderer->selectedParticle = particleIndex;
    }

}

void FluidSolverWindow::UpdateProjectionMatrices() {
    if (scenario != nullptr)
        UpdateParticleRendererProjectionMatrix(scenario->GetParticleCountX(), scenario->GetParticleCountY(),
                                               scenario->GetParticleSize());

}


void FluidSolverWindow::UpdateParticleRendererProjectionMatrix(float particlesX, float particlesY, float particleSize) {

    // This function fits the particle grid into the fbo without distorting it or culling areas off that should be shown

    float width = particlesX; // particle size is not taken into account
    float height = particlesY;

    float fboWidth = particleFBOWidth;
    float fboHeight = particleFBOHeight;

    if (width / fboWidth * fboHeight >= height) {
        height = width / fboWidth * fboHeight;
    } else {
        width = height / fboHeight * fboWidth;
    }

    glm::mat4 generated = ParticleRenderer::GenerateOrtho(-width / 2.0f, width / 2.0f, height / 2.0f, -height / 2.0f);

    if (particleRenderer != nullptr) {
        particleRenderer->projectionMatrix = generated;
    }


}

void FluidSolverWindow::setupFBO() {
    framebuffer = new Engine::Graphics::Framebuffer(particleFBOWidth, particleFBOHeight);
    auto *depthSettings = new Engine::Graphics::Texture2DSettings();
    depthSettings->GenerateMipmaps = false;
    depthSettings->TextureMagnifyingFiltering = GL_NEAREST;
    depthSettings->TextureMinifyingFiltering = GL_NEAREST;
    fboDepthTex = new Engine::Graphics::Texture2D(particleFBOWidth, particleFBOHeight, depthSettings,
                                                  GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT16,
                                                  Engine::ComponentType::ComponentTypeShort);
    framebuffer->AddAttachment(GL_DEPTH_ATTACHMENT, fboDepthTex);

    auto colorSettings = new Engine::Graphics::Texture2DSettings();
    colorSettings->GenerateMipmaps = false;

    fboColorTex = new Engine::Graphics::Texture2D(particleFBOWidth, particleFBOHeight, colorSettings, GL_RGB,
                                                  Engine::ComponentType::ComponentTypeUnsignedByte);
    framebuffer->AddAttachment(GL_COLOR_ATTACHMENT0, fboColorTex);

}

void FluidSolverWindow::saveAsImage() {
    /*if (!saveFrames)
        return;
    if (currentSaveFrameTime < 1.0f / saveFramesPerSecond)
        return;
    currentSaveFrameTime -= 1.0f / saveFramesPerSecond;
    imageCounter++;

    fboColorTex->SaveAsPNG(imagePath + "image_" + std::to_string(imageCounter) + ".png");*/

}

void FluidSolverWindow::setupUI() {

    mainUi = new FluidUI::MainUi(this);

}

void FluidSolverWindow::SetScenario(FluidSolver::Scenario *scenario) {
    if (this->scenario == scenario)
        return;
    this->scenario = scenario;
    resetData();
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

FluidSolverWindow::~FluidSolverWindow() {
    delete mainUi;
    ImGuiHelper::Uninit();
    Engine::Window::~Window();
}

FluidSolver::ISimulationVisualizer *FluidSolverWindow::GetVisualizer() {
    return simulation->getSimulationVisualizer();
}

