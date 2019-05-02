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
#include <core/QuadraticNeighborhoodSearch.hpp>
#include <core/IntegrationSchemeEulerCromer.hpp>


void FluidSolverWindow::render() {
    fpsLabel->setText("FPS: " + std::to_string(GetFPS()));

    if (!this->Pause) {
        accumulatedSimulationTime += GetLastFrameTime() * RealTimeSpeed;
        if (sphFluidSolver != nullptr) {
            while (accumulatedSimulationTime >= sphFluidSolver->TimeStep) {
                //accumulatedSimulationTime -= sphFluidSolver->TimeStep;
                accumulatedSimulationTime = 0.0f; // we always want to render after a simulation step
                sphFluidSolver->ExecuteSimulationStep();
            }
        } else {
            accumulatedSimulationTime = 0.0f;
        }
    }

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
        if (sphFluidSolver != nullptr)
            CalculateCorrectProjectionMatrix(particleCountX, particleCountY, sphFluidSolver->ParticleSize);
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

}

void FluidSolverWindow::loadParticles() {
    loadBoundaryTestExample();
}

void FluidSolverWindow::loadMillionParticleExample() {
    // generate 1 mio particles
    std::vector<FluidSolver::SimpleParticleCollection::FluidParticle> particles(1000 * 1000);
    for (int x = 0; x < 1000; x++) {
        for (int y = 0; y < 1000; y++) {
            FluidSolver::SimpleParticleCollection::FluidParticle p;
            p.Position = glm::vec2((float) x, (float) y);
            p.Velocity = glm::vec2((float) x / 1000.0f, (float) y / 1000.0f);
            p.Acceleration = glm::vec2((float) (1000.0f - x) / 1000.0f, (float) y / 1000.0f);
            p.Pressure = 0.5f;
            p.Density = 10000.0f;
            p.Mass = -4.0f;

            particles[x * 1000 + y] = p;
        }
    }

    auto simple = new FluidSolver::SimpleParticleCollection(particles);
    particleVertexArray = new ParticleVertexArray(simple);

    particleRenderer = new ParticleRenderer(particleVertexArray, ParticleRenderer::GenerateOrtho(0, 1000, 0, 1000));

}

void FluidSolverWindow::loadBoundaryTestExample() {

    // set up basic stuff
    sphFluidSolver = new FluidSolver::SPHFluidSolver();
    sphFluidSolver->TimeStep = 0.001f;

    // set up values
    sphFluidSolver->ParticleSize = 1.0f;
    sphFluidSolver->KernelSupport = 2.0f * sphFluidSolver->ParticleSize;
    sphFluidSolver->NeighborhoodRadius = 2.0f * sphFluidSolver->ParticleSize;
    sphFluidSolver->RestDensity = 1.0f;


    sphFluidSolver->StiffnessK = 100.0f;
    sphFluidSolver->Viscosity = 0.001f;

    sphFluidSolver->kernel = new FluidSolver::CubicSplineKernel();
    sphFluidSolver->neighborhoodSearch = new FluidSolver::QuadraticNeighborhoodSearch();
    sphFluidSolver->integrationScheme = new FluidSolver::IntegrationSchemeEulerCromer();

    resetData();

    // create particle renderer
    particleRenderer = new ParticleRenderer(particleVertexArray, ParticleRenderer::GenerateOrtho(-10, 10, 10, -10));
    particleRenderer->pointSize = 30.0f;
    particleRenderer->colorSelection = ParticleRenderer::ColorSelection::Pressure;
    particleRenderer->topValue = -50.0f;
    particleRenderer->bottomValue = -55.0f;
    particleRenderer->topColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    particleRenderer->bottomColor = glm::vec4(1.0f);
    CalculateCorrectProjectionMatrix(particleCountX, particleCountY, sphFluidSolver->ParticleSize);

    // reset simulation time
    accumulatedSimulationTime = 0.0f;
}

void FluidSolverWindow::resetBoundaryTestExampleData() {
    particleCountX = 20;
    particleCountY = 20;
    CalculateCorrectProjectionMatrix(particleCountX, particleCountY, sphFluidSolver->ParticleSize);

    float mass = sphFluidSolver->RestDensity * sphFluidSolver->ParticleSize * sphFluidSolver->ParticleSize;

    // generate a simple boundary
    std::vector<FluidSolver::SimpleParticleCollection::FluidParticle> particles;
    for (int y = -5; y >= -7; y--) {
        for (int x = -10; x <= 10; x++) {
            FluidSolver::SimpleParticleCollection::FluidParticle p;
            p.Position = glm::vec2((float) x, (float) y);
            p.Velocity = glm::vec2(0.0f);
            p.Acceleration = glm::vec2(0.0f);
            p.Pressure = 0.0f;
            p.Density = sphFluidSolver->RestDensity;
            p.Mass = mass;
            p.Type = FluidSolver::IParticleCollection::ParticleTypeBoundary;
            particles.push_back(p);
        }
    }

    // normal particle
    FluidSolver::SimpleParticleCollection::FluidParticle p;
    p.Position = glm::vec2(0.0f);
    p.Velocity = glm::vec2(0.0f);
    p.Acceleration = glm::vec2(0.0f);
    p.Pressure = 0.0f;
    p.Density = sphFluidSolver->RestDensity;
    p.Mass = mass;
    p.Type = FluidSolver::IParticleCollection::ParticleTypeNormal;
    particles.push_back(p);

    // generate particle collection
    auto simple = new FluidSolver::SimpleParticleCollection(particles);

    // delete old and create new vertex array
    delete particleVertexArray;
    particleVertexArray = new ParticleVertexArray(simple);
    if (particleRenderer != nullptr) {
        particleRenderer->particleVertexArray = particleVertexArray;
    }


    // set up computation providers
    sphFluidSolver->particleCollection = simple;
}

void FluidSolverWindow::resetData() {
    //resetBoundaryTestExampleData();
    //resetSimpleDamExampleData();
    resetHugeDamExampleData();
}

void FluidSolverWindow::resetSimpleDamExampleData() {
    particleCountX = 26;
    particleCountY = 20;
    CalculateCorrectProjectionMatrix(particleCountX, particleCountY, sphFluidSolver->ParticleSize);


    float mass = sphFluidSolver->RestDensity * sphFluidSolver->ParticleSize * sphFluidSolver->ParticleSize;

    // generate a simple boundary
    std::vector<FluidSolver::SimpleParticleCollection::FluidParticle> particles;

    for (int x = -12; x <= 12; x++) {
        FluidSolver::SimpleParticleCollection::FluidParticle p;
        p.Position = glm::vec2((float) x, (float) -5);
        p.Velocity = glm::vec2(0.0f);
        p.Acceleration = glm::vec2(0.0f);
        p.Pressure = 0.0f;
        p.Density = sphFluidSolver->RestDensity;
        p.Mass = mass;
        p.Type = FluidSolver::IParticleCollection::ParticleTypeBoundary;
        particles.push_back(p);

        p.Position = glm::vec2((float) x, -6.0f);
        particles.push_back(p);

        p.Position = glm::vec2((float) x, -7.0f);
        particles.push_back(p);
    }

    // left + right boundary
    for (int y = 5; y > -5; y--) {

        FluidSolver::SimpleParticleCollection::FluidParticle p;
        p.Velocity = glm::vec2(0.0f);
        p.Acceleration = glm::vec2(0.0f);
        p.Pressure = 0.0f;
        p.Density = sphFluidSolver->RestDensity;
        p.Mass = mass;
        p.Type = FluidSolver::IParticleCollection::ParticleTypeBoundary;

        p.Position = glm::vec2((float) -10, (float) y);
        particles.push_back(p);
        p.Position = glm::vec2((float) -11, (float) y);
        particles.push_back(p);
        p.Position = glm::vec2((float) -12, (float) y);
        particles.push_back(p);

        p.Position = glm::vec2((float) 10, (float) y);
        particles.push_back(p);
        p.Position = glm::vec2((float) 11, (float) y);
        particles.push_back(p);
        p.Position = glm::vec2((float) 12, (float) y);
        particles.push_back(p);

    }


    for (int x = -3; x <= 3; x++) {
        for (int y = 3; y >= 0; y--) {


            // normal particle
            FluidSolver::SimpleParticleCollection::FluidParticle p;
            p.Position = glm::vec2((float) x * 2.1f, (float) y * 2.1f);
            p.Velocity = glm::vec2(0.0f);
            p.Acceleration = glm::vec2(0.0f);
            p.Pressure = 0.0f;
            p.Density = sphFluidSolver->RestDensity;
            p.Mass = mass;
            p.Type = FluidSolver::IParticleCollection::ParticleTypeNormal;
            particles.push_back(p);
        }
    }

    // generate particle collection
    auto simple = new FluidSolver::SimpleParticleCollection(particles);

    // delete old and create new vertex array
    delete particleVertexArray;
    particleVertexArray = new ParticleVertexArray(simple);
    if (particleRenderer != nullptr) {
        particleRenderer->particleVertexArray = particleVertexArray;
    }

    // set up computation providers
    sphFluidSolver->particleCollection = simple;
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

void FluidSolverWindow::resetHugeDamExampleData() {
    particleCountX = 106;
    particleCountY = 106;
    CalculateCorrectProjectionMatrix(particleCountX, particleCountY, sphFluidSolver->ParticleSize);

    float mass = sphFluidSolver->RestDensity * sphFluidSolver->ParticleSize * sphFluidSolver->ParticleSize;

    // generate a simple boundary
    std::vector<FluidSolver::SimpleParticleCollection::FluidParticle> particles;

    for (int x = -53; x <= 53; x++) {
        FluidSolver::SimpleParticleCollection::FluidParticle p;

        p.Velocity = glm::vec2(0.0f);
        p.Acceleration = glm::vec2(0.0f);
        p.Pressure = 0.0f;
        p.Density = sphFluidSolver->RestDensity;
        p.Mass = mass;
        p.Type = FluidSolver::IParticleCollection::ParticleTypeBoundary;

        p.Position = glm::vec2((float) x, (float) -50);
        particles.push_back(p);
        p.Position = glm::vec2((float) x, (float) -51);
        particles.push_back(p);
        p.Position = glm::vec2((float) x, (float) -52);
        particles.push_back(p);
    }

    // left boundary
    for (int y = 20; y > -50; y--) {
        FluidSolver::SimpleParticleCollection::FluidParticle p;

        p.Velocity = glm::vec2(0.0f);
        p.Acceleration = glm::vec2(0.0f);
        p.Pressure = 0.0f;
        p.Density = sphFluidSolver->RestDensity;
        p.Mass = mass;
        p.Type = FluidSolver::IParticleCollection::ParticleTypeBoundary;

        p.Position = glm::vec2((float) -50, (float) y);
        particles.push_back(p);
        p.Position = glm::vec2((float) -51, (float) y);
        particles.push_back(p);
        p.Position = glm::vec2((float) -52, (float) y);
        particles.push_back(p);
    }

    // right boundary
    for (int y = 20; y > -50; y--) {
        FluidSolver::SimpleParticleCollection::FluidParticle p;
        p.Velocity = glm::vec2(0.0f);
        p.Acceleration = glm::vec2(0.0f);
        p.Pressure = 0.0f;
        p.Density = sphFluidSolver->RestDensity;
        p.Mass = mass;
        p.Type = FluidSolver::IParticleCollection::ParticleTypeBoundary;

        p.Position = glm::vec2((float) 50, (float) y);
        particles.push_back(p);
        p.Position = glm::vec2((float) 51, (float) y);
        particles.push_back(p);
        p.Position = glm::vec2((float) 52, (float) y);
        particles.push_back(p);

    }


    for (int x = -20; x <= 0; x++) {
        for (int y = 20; y >= 0; y--) {
            // normal particle
            FluidSolver::SimpleParticleCollection::FluidParticle p;
            p.Position = glm::vec2((float) x * 2.1f, (float) y * 2.1f);
            p.Velocity = glm::vec2(0.0f);
            p.Acceleration = glm::vec2(0.0f);
            p.Pressure = 0.0f;
            p.Density = sphFluidSolver->RestDensity;
            p.Mass = mass;
            p.Type = FluidSolver::IParticleCollection::ParticleTypeNormal;
            particles.push_back(p);
        }
    }

    // generate particle collection
    auto simple = new FluidSolver::SimpleParticleCollection(particles);

    // delete old and create new vertex array
    delete particleVertexArray;
    particleVertexArray = new ParticleVertexArray(simple);
    if (particleRenderer != nullptr) {
        particleRenderer->particleVertexArray = particleVertexArray;
    }

    // set up computation providers
    sphFluidSolver->particleCollection = simple;

}
