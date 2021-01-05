#include "GLParticleRenderer.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <engine/Window.hpp>

// shader code
const std::string vertCode = R"(#version 330 core
layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec2 aVelocity;
layout (location = 2) in vec2 aAcceleration;
layout (location = 3) in float aMass;
layout (location = 4) in float aPressure;
layout (location = 5) in float aDensity;
layout (location = 6) in uint aType;
// layout (location = 7) in int aIsSelected;


#define COLOR_SELECTION_VELOCITY 0
#define COLOR_SELECTION_ACCELERATION 1
#define COLOR_SELECTION_MASS 2
#define COLOR_SELECTION_PRESSURE 3
#define COLOR_SELECTION_DENSITY 4

#define PARTICLE_TYPE_BOUNDARY 1u
#define PARTICLE_TYPE_DEAD 2u


uniform int colorSelection;
uniform vec4 bottomColor;
uniform vec4 topColor;
uniform float bottomValue;
uniform float topValue;
uniform vec4 boundaryColor;
uniform int showParticleSelection;
uniform float numberOfParticles;
uniform int showParticleMemoryLocation;


out VS_OUT {
    vec4 color;
    int discarded;
	int selected;
} vs_out;


void main()
{

    vs_out.discarded = 0;
	vs_out.selected = 0;

    // determine selection
    //if(showParticleSelection + aIsSelected == 2){
    //    vs_out.selected = 1;
    //}


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

    if(showParticleMemoryLocation == 1){
        float vertID = float(gl_VertexID);
        float third = vertID / (numberOfParticles / 3.0);
        vs_out.color.r = clamp(third, 0.0, 1.0);
        vs_out.color.g = clamp(third - 1.0f, 0.0, 1.0);
        vs_out.color.b = clamp(third - 2.0f, 0.0, 1.0);
        vs_out.color.a = 1.0;
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







Engine::Graphics::Texture2D* FluidSolver::GLParticleRenderer::get_render_target()
{
	return fboColorTex;
}

void FluidSolver::GLParticleRenderer::initialize()
{
	FLUID_ASSERT(Engine::opengl_context_available());


	FLUID_ASSERT(parameters.collection != nullptr);
	delete particleVertexArray;
	particleVertexArray = ParticleVertexArray::CreateFromParticleCollection(parameters.collection);

	delete particleShader;
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

	create_or_update_fbo();
	calc_projection_matrix();

}

void FluidSolver::GLParticleRenderer::update_data()
{
	FLUID_ASSERT(particleVertexArray != nullptr);
	particleVertexArray->Update(nullptr);
}

void FluidSolver::GLParticleRenderer::render()
{
	FLUID_ASSERT(framebuffer != nullptr);
	FLUID_ASSERT(particleShader != nullptr);
	FLUID_ASSERT(particleVertexArray != nullptr);

	// render particles to fbo
	framebuffer->Bind(true);

	glClearColor(settings.backgroundClearColor.r, settings.backgroundClearColor.g, settings.backgroundClearColor.b, settings.backgroundClearColor.a);
	glClear(GL_COLOR_BUFFER_BIT);


	particleShader->Bind();
	particleShader->SetValue("projectionMatrix", projectionMatrix);
	particleShader->SetValue("pointSize", parameters.particle_size);
	particleShader->SetValue("colorSelection", (int)settings.colorSelection);
	particleShader->SetValue("bottomColor", settings.bottomColor);
	particleShader->SetValue("bottomValue", settings.bottomValue);
	particleShader->SetValue("topColor", settings.topColor);
	particleShader->SetValue("topValue", settings.topValue);
	particleShader->SetValue("boundaryColor", settings.boundaryParticleColor);
	//particleShader->SetValue("showParticleSelection", showParticleSelection ? 1 : 0);
	particleShader->SetValue("numberOfParticles", (float)this->particleVertexArray->GetVaoParticleCount());
	particleShader->SetValue("showParticleMemoryLocation", (int)settings.showMemoryLocation);


	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

	particleVertexArray->Draw();

	//glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);

	// finish up
	framebuffer->Unbind();
	glFlush();
}

FluidSolver::GLParticleRenderer::~GLParticleRenderer()
{
	// cleanup created components
	delete particleVertexArray;
	delete framebuffer;
	delete fboDepthTex;
	delete fboColorTex;
	delete particleShader;
}

void FluidSolver::GLParticleRenderer::create_or_update_fbo()
{
	if (this->fboColorTex != nullptr && this->fboColorTex->getWidth() == parameters.render_target.width && this->fboColorTex->getHeight() == parameters.render_target.height)
		return; // no need to update

	delete fboColorTex;
	delete fboDepthTex;
	delete framebuffer;

	framebuffer = new Engine::Graphics::Framebuffer(parameters.render_target.width, parameters.render_target.height);
	auto* depthSettings = new Engine::Graphics::Texture2DSettings();
	depthSettings->GenerateMipmaps = false;
	depthSettings->TextureMagnifyingFiltering = GL_NEAREST;
	depthSettings->TextureMinifyingFiltering = GL_NEAREST;
	fboDepthTex = new Engine::Graphics::Texture2D(parameters.render_target.width, parameters.render_target.height, depthSettings,
		GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT16,
		Engine::ComponentType::ComponentTypeShort);
	framebuffer->AddAttachment(GL_DEPTH_ATTACHMENT, fboDepthTex);

	auto colorSettings = new Engine::Graphics::Texture2DSettings();
	colorSettings->GenerateMipmaps = false;

	fboColorTex = new Engine::Graphics::Texture2D(parameters.render_target.width, parameters.render_target.height, colorSettings, GL_RGB,
		Engine::ComponentType::ComponentTypeUnsignedByte);
	framebuffer->AddAttachment(GL_COLOR_ATTACHMENT0, fboColorTex);

}

glm::mat4 generate_ortho(float left, float right, float top, float bottom) {
	return glm::ortho((float)left, (float)right, (float)bottom, (float)top);
}


void FluidSolver::GLParticleRenderer::calc_projection_matrix()
{

	// This function fits the particle grid into the fbo without distorting it or culling areas off that should be shown

	float width = parameters.viewport.right - parameters.viewport.left; // particle size is not taken into account
	float height = parameters.viewport.top - parameters.viewport.bottom;

	float fboWidth = parameters.render_target.width;
	float fboHeight = parameters.render_target.height;

	if (width / height * fboHeight > fboWidth) {
		height = width / fboWidth * fboHeight;
	}
	else {
		width = height / fboHeight * fboWidth;
	}

	// top and bottom is swapped, so that everything is rendered correctly (otherwise, we render it upside down)
	glm::mat4 generated = generate_ortho(
		parameters.viewport.left + 0.5f * (parameters.viewport.right - parameters.viewport.left) - width * 0.5f,
		parameters.viewport.left + 0.5f * (parameters.viewport.right - parameters.viewport.left) + width * 0.5f,
		parameters.viewport.top - 0.5f * (parameters.viewport.top - parameters.viewport.bottom) - height * 0.5f,
		parameters.viewport.top - 0.5f * (parameters.viewport.top - parameters.viewport.bottom) + height * 0.5f);

	projectionMatrix = generated;

}

bool FluidSolver::GLRenderer::is_opengl_available()
{
	return Engine::opengl_context_available();
}
