//
// Created by corbi on 17.04.2019.
//

#include "FluidSolverWindow.hpp"
#include "FluidSolverTopMenu.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <engine/EngineException.hpp>
#include <engine/text/FontLoader.hpp>

#include <iostream>
#include <dependencies/cppgui/src/Theme.hpp>
#include <dependencies/cppgui/src/AlignBox.hpp>
#include <dependencies/cppgui/src/Spread.hpp>
#include <core/CubicSplineKernel.hpp>
#include <core/neighborhoodSearch/QuadraticNeighborhoodSearchGreedyAllocated.hpp>
#include <core/IntegrationSchemeEulerCromer.hpp>
#include <core/neighborhoodSearch/QuadraticNeighborhoodSearchPreAllocated.hpp>
#include <core/neighborhoodSearch/HashedNeighborhoodSearch.hpp>


void FluidSolverWindow::render() {
    fpsLabel->setText("FPS: " + std::to_string(GetFPS()));

    if (!this->Pause) {
        accumulatedSimulationTime += GetLastFrameTime() * RealTimeSpeed;
        if (sphFluidSolver != nullptr) {
            while (accumulatedSimulationTime >= sphFluidSolver->TimeStep) {
                //accumulatedSimulationTime -= sphFluidSolver->TimeStep;
                accumulatedSimulationTime = 0.0f; // we always want to render after a simulation step
                sphFluidSolver->ExecuteSimulationStep();
                if (dataLogger)
                    dataLogger->TimeStepPassed();
            }
        } else {
            accumulatedSimulationTime = 0.0f;
        }
    }

    if (infoBox != nullptr)
        infoBox->UpdateData();

    if (particleVertexArray != nullptr)
        particleVertexArray->Update();


    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, GetFramebufferWidth(), GetFramebufferHeight());

    if (particleRenderer != nullptr) {
        if (sphFluidSolver != nullptr)
            particleRenderer->pointSize = sphFluidSolver->ParticleSize;
        particleRenderer->Render();
    }

    uiWrapper->render();
}

FluidSolverWindow::FluidSolverWindow(const std::string &title, int width, int height) : Window(title, width, height) {}

bool FluidSolverWindow::even(int input) {
    if (input < 0)
        throw Engine::EngineException("negative not allowed");
    return input % 2 == 0;
}


void FluidSolverWindow::load() {

    OnKeyPressed.Subscribe([=](int keyCode) { if (keyCode == GLFW_KEY_SPACE)this->Pause = !this->Pause; });
    OnMouseDown.Subscribe([=](Engine::Window::MouseButton btn) {
        this->onClick(this->GetMousePositionX(), this->GetMousePositionY());
    });

    loadParticles();
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

    OnWindowSizeChanged.Subscribe([=](int width, int height) {
        uiWrapper->renderDimensionsUpdated();
        if (scenario != nullptr)
            CalculateCorrectProjectionMatrix(scenario->GetParticleCountX(), scenario->GetParticleCountY(),
                                             scenario->GetParticleSize());
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

    auto alignTop = new cppgui::AlignBox(cppgui::AlignmentTop, 60.0f);
    alignTop->addChild(new cppgui::Spread(new FluidSolverTopMenu(particleRenderer, this)));

    scaff->addChild(alignTop);

    fpsLabel = new cppgui::Label(0, 65, "FPS: ");
    scaff->addChild(fpsLabel);
    fpsLabel->Visible = true;

    infoBox = new FluidSolverParticleInfoGUI(particleCollection->GetSize() - 1, particleCollection);
    infoBox->setLocationX(100);
    infoBox->setLocationY(100);
    infoBox->setWidth(300);
    infoBox->setHeight(300);
    scaff->addChild(infoBox);

    if (particleCollection != nullptr) infoBox->particleCollection = particleCollection;

}

void FluidSolverWindow::loadParticles() {
    // set up basic stuff
    sphFluidSolver = new FluidSolver::SPHFluidSolver();
    sphFluidSolver->TimeStep = 0.00001f;

    // set up values
    sphFluidSolver->KernelSupport = 2.0f * sphFluidSolver->ParticleSize;
    sphFluidSolver->NeighborhoodRadius = 2.0f * sphFluidSolver->ParticleSize;
    sphFluidSolver->RestDensity = 1.0f;


    sphFluidSolver->StiffnessK = 100000.0f;
    sphFluidSolver->Viscosity = 3.0f;

    sphFluidSolver->kernel = new FluidSolver::CubicSplineKernel();
    //sphFluidSolver->neighborhoodSearch = new FluidSolver::QuadraticNeighborhoodSearchGreedyAllocated();
    //sphFluidSolver->neighborhoodSearch = new FluidSolver::QuadraticNeighborhoodSearchPreAllocated();
    sphFluidSolver->neighborhoodSearch = new FluidSolver::HashedNeighborhoodSearch(sphFluidSolver->ParticleSize * 3);
    sphFluidSolver->integrationScheme = new FluidSolver::IntegrationSchemeEulerCromer();

    // set up scenario data
    sphFluidSolver->ParticleSize = scenario->GetParticleSize();
    particleCollection = scenario->GenerateScenario(sphFluidSolver->RestDensity);
    // delete old and create new vertex array
    delete particleVertexArray;
    particleVertexArray = new ParticleVertexArray(
            dynamic_cast<FluidSolver::SimpleParticleCollection *>(particleCollection));
    sphFluidSolver->particleCollection = particleCollection;
    if (infoBox != nullptr) infoBox->particleCollection = particleCollection;


    // create particle renderer
    particleRenderer = new ParticleRenderer(particleVertexArray, ParticleRenderer::GenerateOrtho(-10, 10, 10, -10));
    particleRenderer->pointSize = 30.0f;
    particleRenderer->colorSelection = ParticleRenderer::ColorSelection::Density;
    particleRenderer->topValue = 2.0f;
    particleRenderer->bottomValue = 0.0f;
    particleRenderer->topColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    particleRenderer->bottomColor = glm::vec4(1.0f);
    CalculateCorrectProjectionMatrix(scenario->GetParticleCountX(), scenario->GetParticleCountY(),
                                     scenario->GetParticleSize());
    particleRenderer->particleVertexArray = particleVertexArray;

    // reset simulation time
    accumulatedSimulationTime = 0.0f;

    // setup dataLogger
    dataLogger = new DataLogger(sphFluidSolver, "log.csv");
    dataLogger->StartLogging();
}


void FluidSolverWindow::resetData() {
    if (dataLogger)
        dataLogger->FinishLogging();

    // set up scenario data
    sphFluidSolver->ParticleSize = scenario->GetParticleSize();
    scenario->ResetData(particleCollection, sphFluidSolver->RestDensity);
    // delete old and create new vertex array
    delete particleVertexArray;
    particleVertexArray = new ParticleVertexArray(
            dynamic_cast<FluidSolver::SimpleParticleCollection *>(particleCollection));
    if (particleRenderer != nullptr) {
        particleRenderer->particleVertexArray = particleVertexArray;
    }


    CalculateCorrectProjectionMatrix(scenario->GetParticleCountX(), scenario->GetParticleCountY(),
                                     scenario->GetParticleSize());

    if (dataLogger)
        dataLogger->StartLogging();

}


void FluidSolverWindow::onClick(float x, float y) {

    if (particleCollection == nullptr) return;
    if (particleRenderer == nullptr) return;
    if (infoBox == nullptr) return;

    auto clip = glm::vec2(x, y) / glm::vec2((float) GetWidth(), (float) -GetHeight()) + glm::vec2(0.0f, 1.0f);
    clip *= 2.0;
    clip -= glm::vec2(1.0);

    auto unprojected = glm::inverse(particleRenderer->projectionMatrix) * glm::vec4(clip.x, clip.y, 0.0, 1.0);

    // pos is the position in particle space
    glm::vec2 pos = glm::vec2(unprojected.x, unprojected.y);

    // find nearest particle, that you have clicked on
    uint32_t particleIndex = -1;
    float dist = particleRenderer->pointSize * 0.5;
    for (uint32_t i = 0; i < particleCollection->GetSize(); i++) {
        auto particlePos = particleCollection->GetPosition(i);
        auto partDist = glm::length(pos - particlePos);
        if (partDist < dist) {
            particleIndex = i;
            dist = partDist;
        }
    }

    // set particle index in info box
    if (particleIndex != -1) {
        infoBox->particleIndex = particleIndex;
    }

}


void FluidSolverWindow::CalculateCorrectProjectionMatrix(float particlesX, float particlesY, float particleSize) {

    float width = particlesX * particleSize;
    float height = particlesY * particleSize;

    float screenWidth = GetWidth();
    float screenHeight = GetHeight();

    if (width / screenWidth * screenHeight >= height) {
        height = width / screenWidth * screenHeight;
    } else {
        width = height / screenHeight * screenWidth;
    }

    glm::mat4 generated = ParticleRenderer::GenerateOrtho(-width / 2.0f, width / 2.0f, height / 2.0f, -height / 2.0f);

    if (particleRenderer != nullptr) {
        particleRenderer->projectionMatrix = generated;
    }

}
