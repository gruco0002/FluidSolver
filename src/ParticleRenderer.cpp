//
// Created by corbi on 24.04.2019.
//

#include <libraries/glm/gtc/matrix_transform.hpp>
#include "ParticleRenderer.hpp"

void ParticleRenderer::Render() {
	particleShader->Bind();
	particleShader->SetValue("projectionMatrix", projectionMatrix);
	particleShader->SetValue("pointSize", pointSize);
	particleShader->SetValue("colorSelection", (int)colorSelection);
	particleShader->SetValue("bottomColor", bottomColor);
	particleShader->SetValue("bottomValue", bottomValue);
	particleShader->SetValue("topColor", topColor);
	particleShader->SetValue("topValue", topValue);
	particleShader->SetValue("boundaryColor", boundaryParticleColor);
	if (showParticleSelection) {
		particleShader->SetValue("selectedParticle", selectedParticle);
	}
	else {
		particleShader->SetValue("selectedParticle", -1);
	}

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

	if (particleVertexArray != nullptr)
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
layout (location = 6) in uint aType;


#define COLOR_SELECTION_VELOCITY 1
#define COLOR_SELECTION_ACCELERATION 2
#define COLOR_SELECTION_MASS 3
#define COLOR_SELECTION_PRESSURE 4
#define COLOR_SELECTION_DENSITY 5

#define PARTICLE_TYPE_BOUNDARY 1u
#define PARTICLE_TYPE_DEAD 2u


uniform int colorSelection;
uniform vec4 bottomColor;
uniform vec4 topColor;
uniform float bottomValue;
uniform float topValue;
uniform int selectedParticle;
uniform vec4 boundaryColor;


out VS_OUT {
    vec4 color;
    int discarded;
	int selected;
} vs_out;


void main()
{

    vs_out.discarded = 0;
	vs_out.selected = 0;

	if(gl_VertexID == selectedParticle){
		vs_out.selected = 1;
	}

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

    val = clamp(val - bottomValue, 0.0, (topValue - bottomValue)) / (topValue - bottomValue);
    vs_out.color = mix(bottomColor, topColor, val);

    if(aType == PARTICLE_TYPE_DEAD) {
        vs_out.discarded = 1;
    } else if(aType == PARTICLE_TYPE_BOUNDARY) {
        vs_out.color = boundaryColor;
    }

    gl_Position =  vec4(aPosition, 0.0, 1.0);

}
)";

const std::string fragCode = R"(#version 330 core
out vec4 FragColor;
in vec4 oColor;
in vec2 oTexcoord;
flat in int oSelected;
void main()
{
    const float blurredEdge = 0.025;

    vec2 moved = oTexcoord - vec2(0.5);
    float len = length(moved);
    float edgeMix = clamp(len - (0.5 - blurredEdge), 0.0, blurredEdge) / blurredEdge;

    FragColor = oColor;
    FragColor.a = mix(1.0, 0.0, edgeMix);

	if(oSelected == 1){
		FragColor.r = 1.0;		
	}

}
)";

const std::string geomCode = R"(#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

uniform float pointSize;
uniform mat4 projectionMatrix;

out vec4 oColor;
out vec2 oTexcoord;
flat out int oSelected;

in VS_OUT {
    vec4 color;
    int discarded;
	int selected;
} gs_in[];

void main(){

    if(gs_in[0].discarded == 1) {
        return;
    }

    oColor = gs_in[0].color;
	oSelected = gs_in[0].selected;
    vec4 position = gl_in[0].gl_Position;

    gl_Position = projectionMatrix * (position + vec4(0.5 * pointSize, -0.5 * pointSize, 0.0, 0.0));
    oTexcoord = vec2(1.0, 1.0);
    EmitVertex();
    gl_Position = projectionMatrix * (position + vec4(-0.5 * pointSize, -0.5 * pointSize, 0.0, 0.0));
    oTexcoord = vec2(0.0, 1.0);
    EmitVertex();
    gl_Position = projectionMatrix * (position + vec4(0.5 * pointSize, 0.5 * pointSize, 0.0, 0.0));
    oTexcoord = vec2(1.0, 0.0);
    EmitVertex();
    gl_Position = projectionMatrix * (position + vec4(-0.5 * pointSize, 0.5 * pointSize, 0.0, 0.0));
    oTexcoord = vec2(0.0, 0.0);
    EmitVertex();
    EndPrimitive();


}

)";

void ParticleRenderer::Generate() {
	particleShader = new Engine::Graphics::Shader({
														  Engine::Graphics::Shader::ProgramPart(
																  Engine::Graphics::Shader::ProgramPartTypeVertex,
																  vertCode),
														  Engine::Graphics::Shader::ProgramPart(
																  Engine::Graphics::Shader::ProgramPartTypeGeometry,
																  geomCode),
														  Engine::Graphics::Shader::ProgramPart(
																  Engine::Graphics::Shader::ProgramPartTypeFragment,
																  fragCode),

		});
}

void ParticleRenderer::Delete() {
	delete particleShader;
}

ParticleRenderer::ParticleRenderer(ParticleVertexArray * particleVertexArray, glm::mat4 projectionMatrix) {
	this->particleVertexArray = particleVertexArray;
	this->projectionMatrix = projectionMatrix;
	Generate();
}

glm::mat4 ParticleRenderer::GenerateOrtho(float left, float right, float top, float bottom) {
	return glm::ortho((float)left, (float)right, (float)bottom, (float)top);
}
