//
// Created by corbi on 24.04.2019.
//

#ifndef FLUIDSOLVER_PARTICLERENDERER_HPP
#define FLUIDSOLVER_PARTICLERENDERER_HPP


#include "ParticleVertexArray.hpp"
#include "engine/graphics/Shader.hpp"

class ParticleRenderer {

public:
    enum ColorSelection {
        Velocity = 1,
        Acceleration = 2,
        Mass = 3,
        Pressure = 4,
        Density = 5,
    };


    ParticleRenderer(ParticleVertexArray *particleVertexArray, glm::mat4 projectionMatrix);

    ParticleVertexArray *particleVertexArray = nullptr;

    static glm::mat4 GenerateOrtho(float left, float right, float top, float bottom);

    void Render();

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

private:
    Engine::Graphics::Shader *particleShader = nullptr;

    void Generate();

    void Delete();

};


#endif //FLUIDSOLVER_PARTICLERENDERER_HPP
