//
// Created by corbi on 24.04.2019.
//

#ifndef FLUIDSOLVER_PARTICLERENDERER_HPP
#define FLUIDSOLVER_PARTICLERENDERER_HPP


#include <core/interface/ISimulationVisualizer.hpp>
#include "ParticleVertexArray.hpp"
#include "engine/graphics/Shader.hpp"

class ParticleRenderer : public FluidSolver::ISimulationVisualizer {

public:
    enum ColorSelection {
        Velocity = 0,
        Acceleration = 1,
        Mass = 2,
        Pressure = 3,
        Density = 4,
    };


    explicit ParticleRenderer(glm::mat4 projectionMatrix);


    static glm::mat4 GenerateOrtho(float left, float right, float top, float bottom);

    void Render() override;

    virtual ~ParticleRenderer();

    glm::mat4 projectionMatrix;

    float pointSize = 2.0f;

    ColorSelection colorSelection = ColorSelection::Pressure;

    glm::vec4 bottomColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    float bottomValue = 0.0f;

    glm::vec4 topColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    float topValue = 1.0f;

    int selectedParticle = 0;
    bool showParticleSelection = false;

    glm::vec4 boundaryParticleColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

    glm::vec4 backgroundClearColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);


    void setParticleSize(float particleSize) override;

    float getParticleSize() override;

    float getRestDensity() override;

    void setRestDensity(float restDensity) override;

    void setParticleCollection(FluidSolver::IParticleCollection *particleCollection) override;

    FluidSolver::IParticleCollection *getParticleCollection() override;

private:

    ParticleVertexArray *particleVertexArray = nullptr;

    Engine::Graphics::Shader *particleShader = nullptr;

    FluidSolver::IParticleCollection *ParticleCollection = nullptr;


    float RestDensity = 0.0f;


    void Generate();

    void Delete();

};


#endif //FLUIDSOLVER_PARTICLERENDERER_HPP
