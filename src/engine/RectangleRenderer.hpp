//
// Created by corbi on 22.04.2019.
//

#ifndef FLUIDSOLVER_RECTANGLERENDERER_HPP
#define FLUIDSOLVER_RECTANGLERENDERER_HPP

#include <libraries/glm/glm.hpp>
#include <engine/graphics/Texture2D.hpp>
#include <engine/graphics/Shader.hpp>
#include <engine/graphics/buffer/VertexBuffer.hpp>
#include <engine/graphics/buffer/IndexBuffer.hpp>
#include <engine/graphics/buffer/VertexArray.hpp>

namespace Engine {

    class RectangleRenderer {


    public:

        RectangleRenderer();

        void
        RenderRectangle(glm::vec2 position, glm::vec2 size, glm::vec4 color, glm::vec4 clipArea = glm::vec4(0.0f));

        void
        RenderTexture(glm::vec2 position, glm::vec2 size, Graphics::Texture2D *texture,
                      glm::vec4 clipArea = glm::vec4(0.0f));


        void CreateProjectionMatrixForScreen(float width, float height);

        ~RectangleRenderer();

        glm::mat4 projectionMatrix;

    private:

        void Generate();

        void Delete();

        void UpdateVertices(glm::vec2 position, glm::vec2 size);

        void GenerateShaders();

        Graphics::Shader *simpleRectangleShader;
        Graphics::Shader *textureRectangleShader;
        Graphics::Buffer::VertexBuffer<glm::vec2> *vertexBuffer;
        Graphics::Buffer::VertexBuffer<glm::vec2> *textureCoordBuffer;
        Graphics::Buffer::IndexBuffer<uint8_t> *indexBuffer;
        Graphics::Buffer::VertexArray *vertexArray;

    };

}


#endif //FLUIDSOLVER_RECTANGLERENDERER_HPP