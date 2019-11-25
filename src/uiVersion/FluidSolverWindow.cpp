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
#include <uiVersion/userInterface/RootElement.hpp>
#include <core/basicScenarios/HugeDamScenario.hpp>
#include <core/basicScenarios/SimpleBoxScenario.hpp>
#include <core/basicScenarios/MultipleHoleScenario.hpp>
#include <core/basicScenarios/BoxWithHoleScenario.hpp>


void FluidSolverWindow::render() {

    // particle simulation
    bool simulationStepHappened = false;
    if (simulation != nullptr) {
        if (!this->Pause) {
            accumulatedSimulationTime += GetLastFrameTime() * RealTimeSpeed;

            while (accumulatedSimulationTime >= simulation->getTimestep()) {
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
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

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

    // render fbo to screen
    rectangleRenderer->RenderTexture(glm::vec2(particleFBORect.x, particleFBORect.y),
                                     glm::vec2(particleFBORect.z, particleFBORect.w),
                                     fboColorTex);
    // render ui
    if (uiRunner != nullptr) {
        uiRunner->Update(GetLastFrameTime());
        uiRunner->Render();
    }


}

FluidSolverWindow::FluidSolverWindow(const std::string &title, int width, int height) : Window(title, width, height) {


}


void FluidSolverWindow::load() {

    OnKeyPressed.Subscribe([=](int keyCode) { if (keyCode == GLFW_KEY_SPACE)this->Pause = !this->Pause; });
    OnMouseDown.Subscribe([=](Engine::Window::MouseButton btn) {
        this->onClick(this->GetMousePositionX(), this->GetMousePositionY());
    });
    OnFramebufferSizeChanged.Subscribe([=](int width, int height) { this->UpdateProjectionMatrices(); });
    rectangleRenderer = new Engine::RectangleRenderer();
    setupFBO();
    setupSimulation();
    setupUI();

}

void FluidSolverWindow::setupSimulation() {

    //this->scenario = new FluidSolver::BoxWithHoleScenario();
    this->scenario = new FluidSolver::SimpleBoxScenario();
    //this->scenario = new FluidSolver::MultipleHoleScenario();


    simulation = new FluidSolver::Simulation();

    // set particle size and timestep
    simulation->setTimestep(0.01f);
    simulation->setParticleSize(scenario->GetParticleSize());
    simulation->setRestDensity(1.0f);
    simulation->setGravity(9.81f);

    /*  // set up basic stuff
      auto sphFluidSolver = new FluidSolver::SPHFluidSolver();

      // set up values
      sphFluidSolver->KernelSupport = 2.0f * simulation->getParticleSize();
      sphFluidSolver->NeighborhoodRadius = 2.0f * simulation->getParticleSize();
      sphFluidSolver->StiffnessK = 100000.0f;
      sphFluidSolver->Viscosity = 5.0f;
      sphFluidSolver->kernel = new FluidSolver::CubicSplineKernel();
      sphFluidSolver->neighborhoodSearch = new FluidSolver::HashedNeighborhoodSearch(simulation->getParticleSize() * 3);
      simulation->setFluidSolver(sphFluidSolver);
      */

    auto isphFluidSolver = new FluidSolver::IISPHFluidSolver();
    isphFluidSolver->Omega = 0.5f;
    isphFluidSolver->Gamma = 0.7f;
    isphFluidSolver->MaxDensityErrorAllowed = 0.001f;
    isphFluidSolver->MinNumberOfIterations = 5;
    simulation->setFluidSolver(isphFluidSolver);


    // set up scenario data
    auto particleCollection = scenario->GenerateScenario(simulation->getRestDensity());
    simulation->setParticleCollection(particleCollection);
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
    this->UpdateProjectionMatrices();
    simulation->setSimulationVisualizer(particleRenderer);


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
        fluidSolver->KernelSupport = 2.0f * simulation->getParticleSize();
        fluidSolver->NeighborhoodRadius = 2.0f * simulation->getParticleSize();
    }
    auto ifluidSolver = dynamic_cast<FluidSolver::IISPHFluidSolver *>(simulation->getFluidSolver());
    if (ifluidSolver != nullptr) {

    }

    // set up scenario data
    scenario->ResetData(simulation->getParticleCollection(), simulation->getRestDensity());


    this->UpdateProjectionMatrices();

    if (simulation->getDataLogger())
        simulation->getDataLogger()->StartLogging();

    /* this->imageCounter = 0;
     this->currentSaveFrameTime = 1.0f / this->saveFramesPerSecond;*/

}


void FluidSolverWindow::onClick(float x, float y) {

    if (simulation->getParticleCollection() == nullptr) return;
    if (particleRenderer == nullptr) return;
    if (rectangleRenderer == nullptr) return;


    auto rel = glm::vec2(x, y) / glm::vec2((float) GetWidth(), -(float) GetHeight());
    rel *= 2.0f;
    rel += glm::vec2(-1.0f, 1.0f);
    auto unprojectedTMP = glm::inverse(rectangleRenderer->projectionMatrix) * glm::vec4(rel.x, rel.y, 0.0f, 1.0f);

    auto clip = glm::vec2(unprojectedTMP.x, -unprojectedTMP.y);
    clip *= 2.0;
    clip += glm::vec2(-1.0f, 1.0f);


    auto unprojected = glm::inverse(particleRenderer->projectionMatrix) * glm::vec4(clip.x, clip.y, 0.0f, 1.0f);

    // pos is the position in particle space
    glm::vec2 pos = glm::vec2(unprojected.x, unprojected.y);

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
    UpdateRectangleRendererProjectionMatrix();

}

void FluidSolverWindow::UpdateRectangleRendererProjectionMatrix() {

    // adopt rectangle renderer projection matrix
    float width = GetFramebufferWidth(); // screen width
    float height = GetFramebufferHeight(); // screen height
    glm::mat4 generated = ParticleRenderer::GenerateOrtho(0.0f, width, 0.0f, height);
    if (rectangleRenderer != nullptr)
        rectangleRenderer->projectionMatrix = generated;

    float fboWidth = particleFBOWidth;
    float fboHeight = particleFBOHeight;

    if (fboWidth / fboHeight * height <= width) {
        // fbo fits heightwise, there must be borders left and right
        float newWidth = fboWidth / fboHeight * height;
        float rest = (width - newWidth) / 2.0f;
        particleFBORect = glm::vec4(rest, 0.0f, newWidth, height);
    } else {
        // fbo fits width wise, there must be borders at top and bottom
        float newHeight = fboHeight / fboWidth * width;
        float rest = (height - newHeight) / 2.0f;
        particleFBORect = glm::vec4(0.0f, rest, width, newHeight);
    }


}


void FluidSolverWindow::UpdateParticleRendererProjectionMatrix(float particlesX, float particlesY, float particleSize) {


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
    font = new Engine::Text::Font("../resources/Roboto-Regular.ttf", 88, 5);
    iconFont = new Engine::Text::Font("../resources/MovedMaterialIcons.ttf", 88, 5);
    // create gui interface
    guiInterface = new GuiEngineInterface(this, font, iconFont);

    // create ui entry point element
    entryPoint = new FluidUI::RootElement(this);

    // setup ui runner
    uiRunner = new cppgui::UIRunner(entryPoint, guiInterface);
    uiRunner->setDebugView(false);

    // setup interaction events
    this->OnMouseDown.Subscribe([=](MouseButton b) {
        if (b == LeftButton) {
            uiRunner->interactionRunner->MouseDown(cppgui::MouseButtonLeft);
        } else if (b == RightButton) {
            uiRunner->interactionRunner->MouseDown(cppgui::MouseButtonRight);
        }
    });
    this->OnMouseUp.Subscribe([=](MouseButton b) {
        if (b == LeftButton) {
            uiRunner->interactionRunner->MouseUp(cppgui::MouseButtonLeft);
        } else if (b == RightButton) {
            uiRunner->interactionRunner->MouseUp(cppgui::MouseButtonRight);
        }
    });
    this->OnCursorPositionChanged.Subscribe([=](double x, double y) {
        uiRunner->interactionRunner->MousePosition(cppgui::Vector2(x, y));
    });
    this->OnScrollChanged.Subscribe([=](double x, double y) {
        uiRunner->interactionRunner->MouseScroll((x + y) * -30.0f);
    });

    this->OnTextInput.Subscribe([=](std::string textInput) {
        uiRunner->interactionRunner->TextInput(textInput);
    });
    this->OnKeyPressed.Subscribe([=](int key) {
        if (key == GLFW_KEY_BACKSPACE) {
            uiRunner->interactionRunner->KeyDown(cppgui::KeyboardKeyBackspace);
        } else if (key == GLFW_KEY_ESCAPE) {
            uiRunner->interactionRunner->KeyDown(cppgui::KeyboardKeyEscape);
        } else if (key == GLFW_KEY_LEFT) {
            uiRunner->interactionRunner->KeyDown(cppgui::KeyboardKeyLeft);
        } else if (key == GLFW_KEY_RIGHT) {
            uiRunner->interactionRunner->KeyDown(cppgui::KeyboardKeyRight);
        }
    });
    this->OnKeyRelease.Subscribe([=](int key) {
        if (key == GLFW_KEY_BACKSPACE) {
            uiRunner->interactionRunner->KeyUp(cppgui::KeyboardKeyBackspace);
        } else if (key == GLFW_KEY_ESCAPE) {
            uiRunner->interactionRunner->KeyUp(cppgui::KeyboardKeyEscape);
        } else if (key == GLFW_KEY_LEFT) {
            uiRunner->interactionRunner->KeyUp(cppgui::KeyboardKeyLeft);
        } else if (key == GLFW_KEY_RIGHT) {
            uiRunner->interactionRunner->KeyUp(cppgui::KeyboardKeyRight);
        }
    });
}

