#include "GLParticleRenderer.hpp"

#include <engine/Window.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <incbin/incbin_helper.hpp>

INCBIN(gl_particle_renderer_vertex_shader, "core/visualizer/shader/GLParticleRenderer.vert.glsl");
INCBIN(gl_particle_renderer_geometry_shader, "core/visualizer/shader/GLParticleRenderer.geom.glsl");
INCBIN(gl_particle_renderer_fragment_shader, "core/visualizer/shader/GLParticleRenderer.frag.glsl");

Engine::Graphics::Texture2D* FluidSolver::GLParticleRenderer::get_render_target()
{
    return fboColorTex;
}

void FluidSolver::GLParticleRenderer::initialize()
{
    FLUID_ASSERT(Engine::opengl_context_available());

    initialize_in_next_render_step = true;
}

void FluidSolver::GLParticleRenderer::render()
{
    if (initialize_in_next_render_step)
    {
        initialize_in_next_render_step = false;

        FLUID_ASSERT(parameters.collection != nullptr);
        delete particleVertexArray;
        particleVertexArray = ParticleVertexArray::CreateFromParticleCollection(parameters.collection);

        delete particleShader;
        particleShader = new Engine::Graphics::Shader({
            Engine::Graphics::Shader::ProgramPart(
                Engine::Graphics::Shader::ProgramPartTypeVertex,
                incbin_as_string(g_gl_particle_renderer_vertex_shader_data, g_gl_particle_renderer_vertex_shader_size)),
            Engine::Graphics::Shader::ProgramPart(Engine::Graphics::Shader::ProgramPartTypeGeometry,
                                                  incbin_as_string(g_gl_particle_renderer_geometry_shader_data,
                                                                   g_gl_particle_renderer_geometry_shader_size)),
            Engine::Graphics::Shader::ProgramPart(Engine::Graphics::Shader::ProgramPartTypeFragment,
                                                  incbin_as_string(g_gl_particle_renderer_fragment_shader_data,
                                                                   g_gl_particle_renderer_fragment_shader_size)),
        });

        create_or_update_fbo();
        
    }
    calc_projection_matrix();


    FLUID_ASSERT(framebuffer != nullptr);
    FLUID_ASSERT(particleShader != nullptr);
    FLUID_ASSERT(particleVertexArray != nullptr);
    particleVertexArray->Update(nullptr);

    // render particles to fbo
    framebuffer->Bind(true);

    glClearColor(settings.backgroundClearColor.r, settings.backgroundClearColor.g, settings.backgroundClearColor.b,
                 settings.backgroundClearColor.a);
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
    // particleShader->SetValue("showParticleSelection", showParticleSelection ? 1 : 0);
    particleShader->SetValue("numberOfParticles", (float)this->particleVertexArray->GetVaoParticleCount());
    particleShader->SetValue("showParticleMemoryLocation", (int)settings.showMemoryLocation);


    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

    particleVertexArray->Draw();

    // glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);

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

glm::mat4 generate_ortho(float left, float right, float top, float bottom)
{
    return glm::ortho((float)left, (float)right, (float)bottom, (float)top);
}


void FluidSolver::GLParticleRenderer::calc_projection_matrix()
{

    // This function fits the particle grid into the fbo without distorting it or culling areas off that should be shown

    float width = settings.viewport.right - settings.viewport.left; // particle size is not taken into account
    float height = settings.viewport.top - settings.viewport.bottom;

    float fboWidth = parameters.render_target.width;
    float fboHeight = parameters.render_target.height;

    if (width / height * fboHeight > fboWidth)
    {
        height = width / fboWidth * fboHeight;
    }
    else
    {
        width = height / fboHeight * fboWidth;
    }

    // top and bottom is swapped, so that everything is rendered correctly (otherwise, we render it upside down)
    glm::mat4 generated = generate_ortho(
        settings.viewport.left + 0.5f * (settings.viewport.right - settings.viewport.left) - width * 0.5f,
        settings.viewport.left + 0.5f * (settings.viewport.right - settings.viewport.left) + width * 0.5f,
        settings.viewport.top - 0.5f * (settings.viewport.top - settings.viewport.bottom) - height * 0.5f,
        settings.viewport.top - 0.5f * (settings.viewport.top - settings.viewport.bottom) + height * 0.5f);

    projectionMatrix = generated;
}

FluidSolver::Image FluidSolver::GLParticleRenderer::get_image_data()
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
