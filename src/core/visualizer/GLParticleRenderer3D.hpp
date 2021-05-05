#pragma once

#include "core/visualizer/GLParticleVertexArray3D.hpp"
#include "core/visualizer/GLRenderer.hpp"
#include "core/visualizer/ISimulationVisualizer.hpp"
#include "engine/graphics/Framebuffer.hpp"
#include "engine/graphics/Shader.hpp"
#include "engine/graphics/Texture2D.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace FluidSolver
{

    class GLParticleRenderer3D : public ISimulationVisualizer, public GLRenderer {

      public:
        virtual Engine::Graphics::Texture2D* get_render_target() override;

        virtual void initialize() override;

        virtual void render() override;

        virtual const Image& get_image_data() override;

        virtual ~GLParticleRenderer3D() override;

        struct Settings
        {
            glm::vec4 boundary_particle_color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
            glm::vec4 fluid_particle_color = glm::vec4(0.5f, 0.5f, 1.0f, 1.0f);

            glm::vec4 background_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            glm::mat4 view_matrix =
                glm::lookAt(glm::vec3(0.0f, 1.0f, -1.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        } settings;

      private:
        glm::mat4 projectionMatrix;
        ParticleVertexArray3D* particleVertexArray = nullptr;

        Engine::Graphics::Framebuffer* framebuffer = nullptr;
        Engine::Graphics::Texture2D* fboDepthTex = nullptr;
        Engine::Graphics::Texture2D* fboColorTex = nullptr;

        Engine::Graphics::Shader* particleShader = nullptr;

        bool initialize_in_next_render_step = true;

        void create_or_update_fbo();

        void calc_projection_matrix();
    };



} // namespace FluidSolver