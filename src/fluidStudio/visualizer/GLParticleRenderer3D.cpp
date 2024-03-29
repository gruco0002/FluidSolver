#include "GLParticleRenderer3D.hpp"

#include "Assets.hpp"
#include "engine/Window.hpp"
#include "helpers/Log.hpp"

#include <glm/gtc/matrix_transform.hpp>

Engine::Graphics::Texture2D* LibFluid::GLParticleRenderer3D::get_render_target() {
    return fboColorTex;
}

void LibFluid::GLParticleRenderer3D::initialize() {
    FLUID_ASSERT(Engine::opengl_context_available());

    if (simulation_data.has_data_changed() || parameters.has_data_changed()) {
        initialize_in_next_render_step = true;
    }
}

void LibFluid::GLParticleRenderer3D::create_compatibility_report(CompatibilityReport& report) {
    report.begin_scope(FLUID_NAMEOF(GLParticleRenderer3D));
    if (simulation_data.collection == nullptr) {
        report.add_issue("ParticleCollection is null.");
    } else {
        if (!simulation_data.collection->is_type_present<MovementData3D>()) {
            report.add_issue("Particles are missing the MovementData3D attribute.");
        }
        if (!simulation_data.collection->is_type_present<ParticleInfo>()) {
            report.add_issue("Particles are missing the ParticleInfo attribute.");
        }
    }

    report.end_scope();
}

void LibFluid::GLParticleRenderer3D::render() {
    if (!parameters.enabled) {
        return;
    }

    create_shader_if_required();

    if (initialize_in_next_render_step) {
        initialize_in_next_render_step = false;

        if (simulation_data.has_data_changed()) {
            FLUID_ASSERT(simulation_data.collection != nullptr);
            delete particleVertexArray;
            particleVertexArray = ParticleVertexArray3D::CreateFromParticleCollection(simulation_data.collection);

            simulation_data.acknowledge_data_change();
        }


        if (parameters.has_data_changed()) {
            create_or_update_fbo();
            calc_projection_matrix();

            parameters.acknowledge_data_change();
        }
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
    particleShader->SetValue("viewMatrix", settings.camera.view_matrix());
    particleShader->SetValue("particleColor", settings.fluid_particle_color);
    particleShader->SetValue("boundaryColor", settings.boundary_particle_color);
    particleShader->SetValue("lightDirection", glm::normalize(settings.light_direction));
    particleShader->SetValue("ambientLightFactor", settings.ambient_light_factor);
    particleShader->SetValue("numberOfParticles", (float)simulation_data.collection->size());
    particleShader->SetValue("showParticleMemoryLocation", settings.show_particle_memory_location ? 1 : 0);
    particleShader->SetValue("particleSize", simulation_data.particle_size);
    particleShader->SetValue("selectedTag", (int32_t)settings.selected_tag);

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

LibFluid::GLParticleRenderer3D::~GLParticleRenderer3D() {
    // cleanup created components
    delete particleVertexArray;
    delete framebuffer;
    delete fboDepthTex;
    delete fboColorTex;
    delete particleShader;
}

void LibFluid::GLParticleRenderer3D::create_or_update_fbo() {
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


void LibFluid::GLParticleRenderer3D::calc_projection_matrix() {
    auto flipY =
            glm::mat4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

    // we want to mirror the image at the y-axis, such that it comes out in a "correct" way (we avoid flipping it later)
    projectionMatrix = flipY * glm::perspectiveFov(3.14f * 0.5f, (float)parameters.render_target.width, (float)parameters.render_target.height, 0.1f, 200.0f);
}

LibFluid::Image LibFluid::GLParticleRenderer3D::get_image_data() {
    FLUID_ASSERT(this->fboColorTex != nullptr);

    // texture has 3 channels: rgb
    auto texData = fboColorTex->GetData();

    Image result(fboColorTex->getWidth(), fboColorTex->getHeight());
    for (size_t i = 0; i < texData.size() / 3; i++) {
        FLUID_ASSERT(i < result.size());
        Image::Color c(texData[i * 3 + 0], texData[i * 3 + 1], texData[i * 3 + 2]);
        result.data()[i] = c;
    }
    return result;
}
void LibFluid::GLParticleRenderer3D::create_shader_if_required() {
    if (particleShader != nullptr)
        return;

    try {
        particleShader = new Engine::Graphics::Shader({
                Engine::Graphics::Shader::ProgramPart(
                        Engine::Graphics::Shader::ProgramPartTypeVertex,
                        FluidStudio::Assets::get_string_asset(FluidStudio::Assets::Asset::ParticleRenderer3DVertexShader)),
                Engine::Graphics::Shader::ProgramPart(
                        Engine::Graphics::Shader::ProgramPartTypeGeometry,
                        FluidStudio::Assets::get_string_asset(FluidStudio::Assets::Asset::ParticleRenderer3DGeometryShader)),
                Engine::Graphics::Shader::ProgramPart(
                        Engine::Graphics::Shader::ProgramPartTypeFragment,
                        FluidStudio::Assets::get_string_asset(FluidStudio::Assets::Asset::ParticleRenderer3DFragmentShader)),
        });
    } catch (const std::exception& e) {
        LibFluid::Log::error(std::string("[GLParticleRenderer3D] Could not generate shader for 3D renderer: ") + e.what());
        particleShader = nullptr;
        throw;
    }
}
const glm::mat4& LibFluid::GLParticleRenderer3D::get_projection_matrix() const {
    return projectionMatrix;
}
Engine::Graphics::Framebuffer* LibFluid::GLParticleRenderer3D::get_framebuffer() {
    return framebuffer;
}

void LibFluid::GLParticleRenderer3D::set_view(const glm::vec3& position, const glm::vec3& view_direction, const glm::vec3& view_up) {
    settings.camera.location = position;
    settings.camera.looking_at = position + view_direction;
    settings.camera.up = view_up;
}
void LibFluid::GLParticleRenderer3D::get_view(glm::vec3& position, glm::vec3& view_direction, glm::vec3& view_up) const {
    position = settings.camera.location;
    view_direction = settings.camera.looking_at - settings.camera.location;
    view_up = settings.camera.up;
}
