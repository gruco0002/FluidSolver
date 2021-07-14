#pragma once

#include "engine/graphics/Framebuffer.hpp"
#include "engine/graphics/Shader.hpp"
#include "engine/graphics/Texture2D.hpp"
#include "visualizer/GLParticleVertexArray3D.hpp"
#include "visualizer/GLRenderer.hpp"
#include "visualizer/ISimulationVisualizer.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

namespace FluidSolver
{

    struct Camera3D
    {
        glm::vec3 location;
        glm::vec3 looking_at;
        glm::vec3 up;

        inline glm::mat4 view_matrix() const
        {
            return glm::lookAt(location, looking_at, up);
        }

        inline void move_forward(float amount)
        {
            glm::vec3 forward_vector = glm::normalize(looking_at - location);
            location += amount * forward_vector;
            looking_at += amount * forward_vector;
        }

        inline void move_right(float amount)
        {
            glm::vec3 forward_vector = glm::normalize(looking_at - location);
            glm::vec3 normalized_up = glm::normalize(up);
            glm::vec3 sidwards_vector = glm::cross(forward_vector, normalized_up);
            location += amount * sidwards_vector;
            looking_at += amount * sidwards_vector;
        }

        inline void rotate_horizontal(float amount)
        {
            glm::mat4 rotation = glm::rotate(amount, up);
            glm::vec3 view_vector = looking_at - location;
            glm::vec4 rotated = rotation * glm::vec4(view_vector, 0.0f);
            looking_at = location + glm::vec3(rotated.x, rotated.y, rotated.z);
        }

        inline void rotate_vertical(float amount)
        {
            glm::vec3 forward_vector = glm::normalize(looking_at - location);
            glm::vec3 normalized_up = glm::normalize(up);
            glm::vec3 sidwards_vector = glm::cross(forward_vector, normalized_up);

            glm::mat4 rotation = glm::rotate(amount, sidwards_vector);
            glm::vec3 view_vector = looking_at - location;
            glm::vec4 rotated = rotation * glm::vec4(view_vector, 0.0f);
            looking_at = location + glm::vec3(rotated.x, rotated.y, rotated.z);
        }
    };


    class GLParticleRenderer3D : public ISimulationVisualizer, public GLRenderer {

      public:
        virtual Engine::Graphics::Texture2D* get_render_target() override;

        virtual void initialize() override;

        virtual void render() override;

        virtual Compatibility check() override;

        virtual Image get_image_data() override;

        virtual ~GLParticleRenderer3D() override;

        struct Settings
        {
            glm::vec4 boundary_particle_color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
            glm::vec4 fluid_particle_color = glm::vec4(0.5f, 0.5f, 1.0f, 1.0f);

            glm::vec4 background_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

            Camera3D camera{glm::vec3(7.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -6.0f), glm::vec3(0.0f, 1.0f, 0.0f)};

            glm::vec3 light_direction = glm::vec3(8.0f, -12.0f, 1.0f);

            bool show_particle_memory_location = false;

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