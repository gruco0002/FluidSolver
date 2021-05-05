#include "GLParticleRenderer3D.hpp"

#include <engine/Window.hpp>
#include <glm/gtc/matrix_transform.hpp>

// shader code
const std::string vertCode = R"(#version 330 core
layout (location = 1) in vec3 aPosition;
layout (location = 0) in uint aType;

#define PARTICLE_TYPE_BOUNDARY 1u
#define PARTICLE_TYPE_DEAD 2u

uniform vec4 particleColor;
uniform vec4 boundaryColor;


out VS_OUT {
    vec4 color;
    int discarded;
} vs_out;


void main()
{

    vs_out.discarded = 0;	
   
    vs_out.color = particleColor;

    if(aType == PARTICLE_TYPE_DEAD) {
        vs_out.discarded = 1;
    } else if(aType == PARTICLE_TYPE_BOUNDARY) {
        vs_out.color = boundaryColor;
    }

    
    gl_Position =  vec4(aPosition, 1.0);

}
)";

const std::string fragCode = R"(#version 330 core
out vec4 FragColor;
in vec4 oColor;
in vec2 oTexcoord;
void main()
{
    const float blurredEdge = 0.025;

    vec2 moved = oTexcoord - vec2(0.5);
    float len = length(moved);
    float edgeMix = clamp(len - (0.5 - blurredEdge), 0.0, blurredEdge) / blurredEdge;

    FragColor = oColor;
    FragColor.a = mix(1.0, 0.0, edgeMix);	

}
)";

const std::string geomCode = R"(#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

uniform float pointSize;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

out vec4 oColor;
out vec2 oTexcoord;

in VS_OUT {
    vec4 color;
    int discarded;
} gs_in[];

void main(){

    if(gs_in[0].discarded == 1) {
        return;
    }

    oColor = gs_in[0].color;
    vec4 position = viewMatrix * gl_in[0].gl_Position;

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


Engine::Graphics::Texture2D* FluidSolver::GLParticleRenderer3D::get_render_target()
{
    return fboColorTex;
}

void FluidSolver::GLParticleRenderer3D::initialize()
{
    FLUID_ASSERT(Engine::opengl_context_available());

    initialize_in_next_render_step = true;
}

void FluidSolver::GLParticleRenderer3D::render()
{
    if (initialize_in_next_render_step)
    {
        initialize_in_next_render_step = false;

        FLUID_ASSERT(parameters.collection != nullptr);
        delete particleVertexArray;
        particleVertexArray = ParticleVertexArray3D::CreateFromParticleCollection(parameters.collection);

        delete particleShader;
        particleShader = new Engine::Graphics::Shader({
            Engine::Graphics::Shader::ProgramPart(Engine::Graphics::Shader::ProgramPartTypeVertex, vertCode),
            Engine::Graphics::Shader::ProgramPart(Engine::Graphics::Shader::ProgramPartTypeGeometry, geomCode),
            Engine::Graphics::Shader::ProgramPart(Engine::Graphics::Shader::ProgramPartTypeFragment, fragCode),
        });

        create_or_update_fbo();
        calc_projection_matrix();
    }

    FLUID_ASSERT(framebuffer != nullptr);
    FLUID_ASSERT(particleShader != nullptr);
    FLUID_ASSERT(particleVertexArray != nullptr);
    particleVertexArray->Update();

    // render particles to fbo
    framebuffer->Bind(true);

    glClearColor(settings.background_color.r, settings.background_color.g, settings.background_color.b,
                 settings.background_color.a);
    glClear(GL_COLOR_BUFFER_BIT);


    // set uniforms of particle shader
    particleShader->Bind();
    particleShader->SetValue("projectionMatrix", projectionMatrix);
    particleShader->SetValue("viewMatrix", settings.view_matrix);
    particleShader->SetValue("pointSize", parameters.particle_size);
    particleShader->SetValue("particleColor", settings.fluid_particle_color);
    particleShader->SetValue("boundaryColor", settings.boundary_particle_color);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

    particleVertexArray->Draw();

    // glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);

    // finish up
    framebuffer->Unbind();
    glFlush();
}

FluidSolver::GLParticleRenderer3D::~GLParticleRenderer3D()
{
    // cleanup created components
    delete particleVertexArray;
    delete framebuffer;
    delete fboDepthTex;
    delete fboColorTex;
    delete particleShader;
}

void FluidSolver::GLParticleRenderer3D::create_or_update_fbo()
{
    if (this->fboColorTex != nullptr && this->fboColorTex->getWidth() == parameters.render_target.width &&
        this->fboColorTex->getHeight() == parameters.render_target.height)
        return; // no need to update

    delete fboColorTex;
    delete fboDepthTex;
    delete framebuffer;

    framebuffer = new Engine::Graphics::Framebuffer(parameters.render_target.width, parameters.render_target.height);
    auto* depthSettings = new Engine::Graphics::Texture2DSettings();
    depthSettings->GenerateMipmaps = false;
    depthSettings->TextureMagnifyingFiltering = GL_NEAREST;
    depthSettings->TextureMinifyingFiltering = GL_NEAREST;
    fboDepthTex = new Engine::Graphics::Texture2D(parameters.render_target.width, parameters.render_target.height,
                                                  depthSettings, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT16,
                                                  Engine::ComponentType::ComponentTypeShort);
    framebuffer->AddAttachment(GL_DEPTH_ATTACHMENT, fboDepthTex);

    auto colorSettings = new Engine::Graphics::Texture2DSettings();
    colorSettings->GenerateMipmaps = false;

    fboColorTex =
        new Engine::Graphics::Texture2D(parameters.render_target.width, parameters.render_target.height, colorSettings,
                                        GL_RGB, Engine::ComponentType::ComponentTypeUnsignedByte);
    framebuffer->AddAttachment(GL_COLOR_ATTACHMENT0, fboColorTex);
}


void FluidSolver::GLParticleRenderer3D::calc_projection_matrix()
{
    projectionMatrix = glm::perspectiveFov(90.0f, (float)parameters.render_target.width,
                                           (float)parameters.render_target.height, 0.01f, 100.0f);
}

const FluidSolver::Image& FluidSolver::GLParticleRenderer3D::get_image_data()
{
    FLUID_ASSERT(this->fboColorTex != nullptr);

    // texture has 3 channels: rgb
    auto texData = fboColorTex->GetData();

    Image result(fboColorTex->getWidth(), fboColorTex->getHeight());
    for (size_t i = 0; i < texData.size() / 3; i++)
    {
        FLUID_ASSERT(i < result.size());
        Image::Color c(texData[i * 3 + 0], texData[i * 3 + 1], texData[i * 3 + 2]);
        result.data()[i] = c;
    }
    return result;
}
