#include "glm/gtc/matrix_transform.hpp"
#include "ParticleRenderer.hpp"


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






ParticleRenderer::ParticleRenderer() {
    Generate();
    RecreateFBOStuff();
    CalculateProjectionMatrix();
}

ParticleRenderer::~ParticleRenderer() {
    delete particleShader;
    delete fboColorTex;
    delete fboDepthTex;
    delete framebuffer;
    delete particleVertexArray;
}

void ParticleRenderer::initialize()
{
    delete particleVertexArray;
    particleVertexArray = ParticleVertexArray::CreateFromParticleCollection(parameters.collection);
    this->pointSize = parameters.particle_size;

    
    if (this->renderTargetWidth != parameters.render_target.width || this->renderTargetHeight != parameters.render_target.height) {
        this->renderTargetWidth = parameters.render_target.width;
        this->renderTargetHeight = parameters.render_target.height;
        RecreateFBOStuff();
    }
    CalculateProjectionMatrix();

}

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


void ParticleRenderer::render() {

    // render particles to fbo
    framebuffer->Bind(true);

    glClearColor(backgroundClearColor.r, backgroundClearColor.g, backgroundClearColor.b, backgroundClearColor.a);
    glClear(GL_COLOR_BUFFER_BIT);

    //particleVertexArray->Update(nullptr);

    particleShader->Bind();
    particleShader->SetValue("projectionMatrix", projectionMatrix);
    particleShader->SetValue("pointSize", pointSize);
    particleShader->SetValue("colorSelection", (int) colorSelection);
    particleShader->SetValue("bottomColor", bottomColor);
    particleShader->SetValue("bottomValue", bottomValue);
    particleShader->SetValue("topColor", topColor);
    particleShader->SetValue("topValue", topValue);
    particleShader->SetValue("boundaryColor", boundaryParticleColor);
    //particleShader->SetValue("showParticleSelection", showParticleSelection ? 1 : 0);
    particleShader->SetValue("numberOfParticles", (float) this->particleVertexArray->GetVaoParticleCount());
    particleShader->SetValue("showParticleMemoryLocation", (int) showMemoryLocation);


    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

    if (particleVertexArray != nullptr)
        particleVertexArray->Draw();

    //glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);

    // finish up
    framebuffer->Unbind();
    glFlush();
}




glm::mat4 ParticleRenderer::GenerateOrtho(float left, float right, float top, float bottom) {
    return glm::ortho((float) left, (float) right, (float) bottom, (float) top);
}

void ParticleRenderer::CalculateProjectionMatrix() {
    // This function fits the particle grid into the fbo without distorting it or culling areas off that should be shown

    float width = parameters.viewport.right - parameters.viewport.left; // particle size is not taken into account
    float height = parameters.viewport.top - parameters.viewport.bottom;

    float fboWidth = renderTargetWidth;
    float fboHeight = renderTargetHeight;

    if (width / height * fboHeight > fboWidth) {
        height = width / fboWidth * fboHeight;
    } else {
        width = height / fboHeight * fboWidth;
    }

    // top and bottom is swapped, so that everything is rendered correctly (otherwise, we render it upside down)
    glm::mat4 generated = ParticleRenderer::GenerateOrtho(
        parameters.viewport.left + 0.5f * (parameters.viewport.right - parameters.viewport.left) - width * 0.5f,
        parameters.viewport.left + 0.5f * (parameters.viewport.right - parameters.viewport.left) + width * 0.5f,
        parameters.viewport.top - 0.5f * (parameters.viewport.top - parameters.viewport.bottom) - height * 0.5f,
        parameters.viewport.top - 0.5f * (parameters.viewport.top - parameters.viewport.bottom) + height * 0.5f);

    projectionMatrix = generated;

}

void ParticleRenderer::RecreateFBOStuff() {
    delete fboColorTex;
    delete fboDepthTex;
    delete framebuffer;

    framebuffer = new Engine::Graphics::Framebuffer(renderTargetWidth, renderTargetHeight);
    auto *depthSettings = new Engine::Graphics::Texture2DSettings();
    depthSettings->GenerateMipmaps = false;
    depthSettings->TextureMagnifyingFiltering = GL_NEAREST;
    depthSettings->TextureMinifyingFiltering = GL_NEAREST;
    fboDepthTex = new Engine::Graphics::Texture2D(renderTargetWidth, renderTargetHeight, depthSettings,
                                                  GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT16,
                                                  Engine::ComponentType::ComponentTypeShort);
    framebuffer->AddAttachment(GL_DEPTH_ATTACHMENT, fboDepthTex);

    auto colorSettings = new Engine::Graphics::Texture2DSettings();
    colorSettings->GenerateMipmaps = false;

    fboColorTex = new Engine::Graphics::Texture2D(renderTargetWidth, renderTargetHeight, colorSettings, GL_RGB,
                                                  Engine::ComponentType::ComponentTypeUnsignedByte);
    framebuffer->AddAttachment(GL_COLOR_ATTACHMENT0, fboColorTex);
}

Engine::Graphics::Texture2D *ParticleRenderer::GetTexture() {
    return fboColorTex;
}

glm::vec2 ParticleRenderer::ConvertPixelCoordinateToParticleSpace(size_t pixelX, size_t pixelY) {
    auto rel = glm::vec2((float) pixelX, (float) pixelY) /
               glm::vec2((float) renderTargetWidth,
                         ((float) renderTargetHeight));
    rel *= 2.0f;
    rel += glm::vec2(-1.0f, -1.0f);
    auto unprojected = glm::inverse(projectionMatrix) * glm::vec4(rel.x, rel.y, 0.0f, 1.0f);

    // pos is the position in particle space
    glm::vec2 pos = glm::vec2(unprojected.x, unprojected.y);
    return pos;
}

void ParticleRenderer::update_data() {
    FLUID_ASSERT(particleVertexArray != nullptr);
    particleVertexArray->Update(nullptr);
}
