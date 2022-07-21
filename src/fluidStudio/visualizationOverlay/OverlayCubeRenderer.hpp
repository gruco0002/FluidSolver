#pragma once

#include "engine/graphics/Framebuffer.hpp"
#include "engine/graphics/Shader.hpp"
#include "engine/graphics/buffer/IndexBuffer.hpp"
#include "engine/graphics/buffer/VertexArray.hpp"
#include "engine/graphics/buffer/VertexBuffer.hpp"

#include <glm/glm.hpp>
#include <memory>

namespace FluidStudio {
    class OverlayCubeRenderer {
      public:
        struct {
            Engine::Graphics::Framebuffer* framebuffer = nullptr;

            glm::mat4 projection_matrix = glm::mat4(1.0f);
            glm::mat4 view_matrix = glm::mat4(1.0f);


        } data;

        void render(const glm::mat4& model_matrix, float opacity = 0.4f);

        void render(const glm::vec3& center, const glm::vec3 distance_from_center, float opacity = 0.4f);



      private:


        struct {
            std::unique_ptr<Engine::Graphics::Buffer::VertexArray> cube_va = nullptr;
            std::unique_ptr<Engine::Graphics::Buffer::IndexBuffer<uint32_t>> index_buffer = nullptr;
            std::unique_ptr<Engine::Graphics::Buffer::VertexBuffer<glm::vec3>> vertex_buffer = nullptr;
            std::unique_ptr<Engine::Graphics::Buffer::VertexBuffer<uint32_t>> face_id_buffer = nullptr;

            std::unique_ptr<Engine::Graphics::Shader> shader = nullptr;
        } cube_va;


        void initialize();

        void create_cube_va();
    };
} // namespace FluidUi
