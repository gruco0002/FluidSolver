#include "GLParticleRenderer3D.hpp"

#include "core/Log.hpp"
#include "engine/Window.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <incbin/incbin_helper.hpp>

INCBIN(gl_particle_renderer_3d_vertex_shader, "core/visualizer/shader/GLParticleRenderer3D.vert.glsl");
INCBIN(gl_particle_renderer_3d_geometry_shader, "core/visualizer/shader/GLParticleRenderer3D.geom.glsl");
INCBIN(gl_particle_renderer_3d_fragment_shader, "core/visualizer/shader/GLParticleRenderer3D.frag.glsl");


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
        try
        {
            particleShader = new Engine::Graphics::Shader({
                Engine::Graphics::Shader::ProgramPart(Engine::Graphics::Shader::ProgramPartTypeVertex,
                                                      incbin_as_string(g_gl_particle_renderer_3d_vertex_shader_data,
                                                                       g_gl_particle_renderer_3d_vertex_shader_size)),
                Engine::Graphics::Shader::ProgramPart(Engine::Graphics::Shader::ProgramPartTypeGeometry,
                                                      incbin_as_string(g_gl_particle_renderer_3d_geometry_shader_data,
                                                                       g_gl_particle_renderer_3d_geometry_shader_size)),
                Engine::Graphics::Shader::ProgramPart(Engine::Graphics::Shader::ProgramPartTypeFragment,
                                                      incbin_as_string(g_gl_particle_renderer_3d_fragment_shader_data,
                                                                       g_gl_particle_renderer_3d_fragment_shader_size)),
            });
        }
        catch (const Engine::EngineException& e)
        {
            FluidSolver::Log::error("[GLParticleRenderer3D] Could not generate shader for 3D renderer: " + e.msg);
            particleShader = nullptr;
        }


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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // set uniforms of particle shader
    particleShader->Bind();
    particleShader->SetValue("projectionMatrix", projectionMatrix);
    particleShader->SetValue("viewMatrix", settings.view_matrix);
    particleShader->SetValue("pointSize", parameters.particle_size);
    particleShader->SetValue("particleColor", settings.fluid_particle_color);
    particleShader->SetValue("boundaryColor", settings.boundary_particle_color);
    particleShader->SetValue("lightDirection", glm::normalize(settings.light_direction));
    particleShader->SetValue("ambientLightFactor", 0.1f);

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
    auto flipY =
        glm::mat4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

    // we want to mirror the image at the y-axis, such that it comes out in a "correct" way (we avoid flipping it later)
    projectionMatrix = flipY * glm::perspectiveFov(3.14f * 0.5f, (float)parameters.render_target.width,
                                                   (float)parameters.render_target.height, 0.01f, 100.0f);
}

FluidSolver::Image FluidSolver::GLParticleRenderer3D::get_image_data()
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
