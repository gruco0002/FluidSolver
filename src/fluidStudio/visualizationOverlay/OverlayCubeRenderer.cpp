#include "OverlayCubeRenderer.hpp"
#include <glm/ext/matrix_transform.hpp>

namespace FluidUi {

    void OverlayCubeRenderer::render(const glm::mat4& model_matrix, float opacity) {
        initialize();

        data.framebuffer->Bind(true);

        cube_va.shader->SetValue("projectionMatrix", data.projection_matrix);
        cube_va.shader->SetValue("viewMatrix", data.view_matrix);
        cube_va.shader->SetValue("modelMatrix", model_matrix);
        cube_va.shader->SetValue("opacity", opacity);

        cube_va.shader->Bind();

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        cube_va.cube_va->Draw(GL_TRIANGLES, 6 * 2 * 3);

        cube_va.shader->Unbind();
        data.framebuffer->Unbind();
        glFlush();
    }

    void OverlayCubeRenderer::initialize() {
        if (cube_va.cube_va == nullptr) {
            create_cube_va();
        }
    }
    void OverlayCubeRenderer::create_cube_va() {
        std::vector<uint32_t> face_id {
                0, 0, 0, 0, 0, 0,
                1, 1, 1, 1, 1, 1,
                2, 2, 2, 2, 2, 2,
                3, 3, 3, 3, 3, 3,
                4, 4, 4, 4, 4, 4,
                5, 5, 5, 5, 5, 5

        };

        std::vector<glm::vec3>
                cube_vertices {
                        {
                                -1.0f, //-x
                                -1.0f,
                                -1.0f,
                        },
                        {
                                -1.0f,
                                -1.0f,
                                1.0f,
                        },
                        {
                                -1.0f,
                                1.0f,
                                1.0f,
                        },
                        {
                                -1.0f, // -x
                                -1.0f,
                                -1.0f,
                        },
                        {
                                -1.0f,
                                1.0f,
                                1.0f,
                        },
                        {
                                -1.0f,
                                1.0f,
                                -1.0f,
                        },
                        {
                                1.0f, // -z
                                1.0f,
                                -1.0f,
                        },
                        {
                                -1.0f,
                                -1.0f,
                                -1.0f,
                        },
                        {
                                -1.0f,
                                1.0f,
                                -1.0f,
                        },

                        {
                                1.0f, // -z
                                1.0f,
                                -1.0f,
                        },
                        {
                                1.0f,
                                -1.0f,
                                -1.0f,
                        },
                        {
                                -1.0f,
                                -1.0f,
                                -1.0f,
                        },

                        {
                                1.0f, // -y
                                -1.0f,
                                1.0f,
                        },
                        {
                                -1.0f,
                                -1.0f,
                                -1.0f,
                        },
                        {
                                1.0f,
                                -1.0f,
                                -1.0f,
                        },
                        {
                                1.0f, // -y
                                -1.0f,
                                1.0f,
                        },
                        {
                                -1.0f,
                                -1.0f,
                                1.0f,
                        },
                        {
                                -1.0f,
                                -1.0f,
                                -1.0f,
                        },

                        {
                                1.0f, // +x
                                1.0f,
                                1.0f,
                        },
                        {
                                1.0f,
                                -1.0f,
                                -1.0f,
                        },
                        {
                                1.0f,
                                1.0f,
                                -1.0f,
                        },
                        {
                                1.0f, //+x
                                -1.0f,
                                -1.0f,
                        },
                        {
                                1.0f,
                                1.0f,
                                1.0f,
                        },
                        {
                                1.0f,
                                -1.0f,
                                1.0f,
                        },
                        {
                                1.0f, // +y
                                1.0f,
                                1.0f,
                        },
                        {
                                1.0f,
                                1.0f,
                                -1.0f,
                        },
                        {
                                -1.0f,
                                1.0f,
                                -1.0f,
                        },
                        {
                                1.0f, // +y
                                1.0f,
                                1.0f,
                        },
                        {
                                -1.0f,
                                1.0f,
                                -1.0f,
                        },
                        {
                                -1.0f,
                                1.0f,
                                1.0f,
                        },
                        {
                                1.0f, // +z
                                1.0f,
                                1.0f,
                        },
                        {
                                -1.0f,
                                1.0f,
                                1.0f,
                        },
                        {1.0f, -1.0f, 1.0f},
                        {
                                -1.0f, // +z
                                1.0f,
                                1.0f,
                        },
                        {
                                -1.0f,
                                -1.0f,
                                1.0f,
                        },
                        {
                                1.0f,
                                -1.0f,
                                1.0f,
                        }};

        std::vector<uint32_t> cube_indices;
        for (size_t i = 0; i < cube_vertices.size(); i++) {
            cube_indices.push_back(i);
        }

        cube_va.vertex_buffer = std::make_unique<Engine::Graphics::Buffer::VertexBuffer<glm::vec3>>(cube_vertices);
        cube_va.face_id_buffer = std::make_unique<Engine::Graphics::Buffer::VertexBuffer<uint32_t>>(face_id);
        cube_va.index_buffer = std::make_unique<Engine::Graphics::Buffer::IndexBuffer<uint32_t>>(cube_indices);

        std::vector<Engine::Graphics::Buffer::VertexArray::BufferBinding> bindings;

        bindings.emplace_back(&(*cube_va.vertex_buffer), 0, 3, 0, sizeof(float) * 3, Engine::ComponentTypeFloat, false);
        bindings.emplace_back(&(*cube_va.face_id_buffer), 1, 1, 0, sizeof(uint32_t), Engine::ComponentTypeUInt, false);
        bindings.emplace_back(&(*cube_va.index_buffer), 0, 1, 0, sizeof(uint32_t), Engine::ComponentTypeUInt, false);

        cube_va.cube_va = std::make_unique<Engine::Graphics::Buffer::VertexArray>(bindings);


        // create shader
        std::vector<Engine::Graphics::Shader::ProgramPart> shader_parts {
                {Engine::Graphics::Shader::ProgramPartType::ProgramPartTypeVertex,
                        R"SHADER(#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in uint aFaceId;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;
uniform float opacity;

out vec4 color;

void main()
{
    color = vec4(vec3((float(aFaceId) + 1.0f) / 6.0f), opacity);
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(aPosition, 1.0);
}

)SHADER"},

                {Engine::Graphics::Shader::ProgramPartType::ProgramPartTypeFragment,
                        R"SHADER(#version 330 core

in vec4 color;
out vec4 FragColor;

void main()
{
    FragColor = color;
}

)SHADER"}};
        cube_va.shader = std::make_unique<Engine::Graphics::Shader>(shader_parts);
    }


    void OverlayCubeRenderer::render(const glm::vec3& center, const glm::vec3 distance_from_center, float opacity) {
        glm::mat4 model_matrix = glm::translate(glm::scale(glm::mat4(1.0f), distance_from_center), center);
        render(model_matrix, opacity);
    }


} // namespace FluidUi