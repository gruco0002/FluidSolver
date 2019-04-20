//
// Created by corbi on 20.04.2019.
//

#ifndef FLUIDSOLVER_TEXTRENDERER_HPP
#define FLUIDSOLVER_TEXTRENDERER_HPP

#include <engine/graphics/buffer/VertexBuffer.hpp>
#include <engine/graphics/buffer/IndexBuffer.hpp>
#include <engine/graphics/buffer/VertexArray.hpp>
#include <engine/graphics/Shader.hpp>
#include "Font.hpp"

namespace Engine {
    namespace Text {
        class TextRenderer {


        public:
            TextRenderer(Font *font);

            void Render(std::string &text, float size, glm::vec2 position, glm::vec4 color);

            glm::mat4 projectionMatrix;

            ~TextRenderer();

            float distanceFieldWidth = 0.46f;
            float distanceFieldEdge = 0.19f;

        private:

            void Generate();

            void Delete();


            void GenerateShader();

            Font *font;

            Graphics::Shader *textShader;
            Graphics::Buffer::VertexBuffer<TextVertex> *vertexBuffer;
            Graphics::Buffer::IndexBuffer<uint16_t> *indexBuffer;
            Graphics::Buffer::VertexArray *vertexArray;


        };
    }
}

#endif //FLUIDSOLVER_TEXTRENDERER_HPP
