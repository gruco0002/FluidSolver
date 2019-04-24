//
// Created by corbi on 24.04.2019.
//

#include <libraries/glm/gtc/matrix_transform.hpp>
#include "ParticleRenderer.hpp"

void ParticleRenderer::Render() {
    particleShader->Bind();
    particleShader->SetValue("projectionMatrix", projectionMatrix);
    particleShader->SetValue("pointSize", pointSize);
    particleShader->SetValue("colorSelection", (int) colorSelection);
    particleShader->SetValue("bottomColor", bottomColor);
    particleShader->SetValue("bottomValue", bottomValue);
    particleShader->SetValue("topColor", topColor);
    particleShader->SetValue("topValue", topValue);


    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

    particleVertexArray->Draw();

    //glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
}

ParticleRenderer::~ParticleRenderer() {
    Delete();
}

const std::string vertCode = R"(#version 330 core
layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec2 aVelocity;
layout (location = 2) in vec2 aAcceleration;
layout (location = 3) in float aMass;
layout (location = 4) in float aPressure;
layout (location = 5) in float aDensity;


#define COLOR_SELECTION_VELOCITY 1
#define COLOR_SELECTION_ACCELERATION 2
#define COLOR_SELECTION_MASS 3
#define COLOR_SELECTION_PRESSURE 4
#define COLOR_SELECTION_DENSITY 5


uniform mat4 projectionMatrix;
uniform float pointSize;

uniform int colorSelection;
uniform vec4 bottomColor;
uniform vec4 topColor;
uniform float bottomValue;
uniform float topValue;


out vec4 oColor;

void main()
{

    float val = 0.0;
    if(colorSelection == COLOR_SELECTION_VELOCITY){
        val = length(aVelocity);
    } else if(colorSelection == COLOR_SELECTION_ACCELERATION){
        val = length(aAcceleration);
    } else if(colorSelection == COLOR_SELECTION_MASS){
        val = aMass;
    } else if(colorSelection == COLOR_SELECTION_PRESSURE){
        val = aPressure;
    } else if(colorSelection == COLOR_SELECTION_DENSITY){
        val = aDensity;
    }

    val = clamp(val, bottomValue, topValue) / (topValue - bottomValue);
    oColor = mix(bottomColor, topColor, val);

    gl_Position = projectionMatrix * vec4(aPosition, 0.0, 1.0);
    gl_PointSize = pointSize;
}
)";

const std::string fragCode = R"(#version 330 core
out vec4 FragColor;
in vec4 oColor;
void main()
{
    FragColor = oColor;
}
)";

void ParticleRenderer::Generate() {
    particleShader = new Engine::Graphics::Shader({
                                                          Engine::Graphics::Shader::ProgramPart(
                                                                  Engine::Graphics::Shader::ProgramPartTypeVertex,
                                                                  vertCode),
                                                          Engine::Graphics::Shader::ProgramPart(
                                                                  Engine::Graphics::Shader::ProgramPartTypeFragment,
                                                                  fragCode),

                                                  });
}

void ParticleRenderer::Delete() {
    delete particleShader;
}

ParticleRenderer::ParticleRenderer(ParticleVertexArray *particleVertexArray, glm::mat4 projectionMatrix) {
    this->particleVertexArray = particleVertexArray;
    this->projectionMatrix = projectionMatrix;
    Generate();
}

glm::mat4 ParticleRenderer::GenerateOrtho(float left, float right, float top, float bottom) {
    return glm::ortho((float) left, (float) right, (float) bottom, (float) top);
}
