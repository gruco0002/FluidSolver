#ifndef FLUIDSOLVER_PARTICLERENDERER_HPP
#define FLUIDSOLVER_PARTICLERENDERER_HPP


#include <core/visualizer/ISimulationVisualizer.hpp>
#include <engine/graphics/Framebuffer.hpp>
#include "ParticleVertexArray.hpp"
#include "engine/graphics/Shader.hpp"
#include "IOpenGLVisualizer.hpp"

class ParticleRenderer : public FluidSolver::ISimulationVisualizer, public IOpenGLVisualizer {
public:
    Engine::Graphics::Texture2D *GetTexture() override;

public:
    enum ColorSelection {
        Velocity = 0,
        Acceleration = 1,
        Mass = 2,
        Pressure = 3,
        Density = 4,
    };


    ParticleRenderer();


    static glm::mat4 GenerateOrtho(float left, float right, float top, float bottom);

    void Render() override;

    virtual ~ParticleRenderer();


    float pointSize = 30.0f;

    ColorSelection colorSelection = ColorSelection::Density;

    glm::vec4 bottomColor = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
    float bottomValue = 0.0f;

    glm::vec4 topColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    float topValue = 2.0f;


    bool showParticleSelection = false;

    glm::vec4 boundaryParticleColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

    glm::vec4 backgroundClearColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    bool showMemoryLocation = false;


    void setParticleSize(float particleSize) override;

    glm::vec2 ConvertPixelCoordinateToParticleSpace(size_t pixelX, size_t pixelY) override;

    float getParticleSize() override;

    float getRestDensity() override;

    void setRestDensity(float restDensity) override;

    void setParticleCollection(FluidSolver::IParticleCollection *particleCollection) override;

    FluidSolver::IParticleCollection *getParticleCollection() override;

    void setSimulationViewArea(SimulationViewArea viewArea) override;

    void setRenderTargetSize(size_t width, size_t height) override;

private:

    FluidSolver::IParticleSelection *particleSelection = nullptr;
public:
    void setParticleSelection(FluidSolver::IParticleSelection *particleSelection) override;

    FluidSolver::IParticleSelection *getParticleSelection() override;

private:

    SimulationViewArea viewArea;
    size_t renderTargetWidth = 1920;
    size_t renderTargetHeight = 1080;

    void CalculateProjectionMatrix();

    glm::mat4 projectionMatrix;
    ParticleVertexArray *particleVertexArray = nullptr;

    Engine::Graphics::Framebuffer *framebuffer = nullptr;
    Engine::Graphics::Texture2D *fboDepthTex = nullptr;
    Engine::Graphics::Texture2D *fboColorTex = nullptr;

    Engine::Graphics::Shader *particleShader = nullptr;

    FluidSolver::IParticleCollection *ParticleCollection = nullptr;


    float RestDensity = 0.0f;

    void RecreateFBOStuff();

    void Generate();


};


#endif //FLUIDSOLVER_PARTICLERENDERER_HPP
