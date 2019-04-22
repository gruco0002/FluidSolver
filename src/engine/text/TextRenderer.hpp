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

            /**
             * Renders a text to the bound framebuffer. All values are in pixel dimensions. The origin (0,0) is on the top left.
             * @param text The text that should be rendered.
             * @param size The font size in pixels.
             * @param position The position of the text.
             * @param color Color of the text in rgba. Reaching from 0.0f to 1.0f
             * @param clipArea Rectangle in which the text will be rendered. Outside of the rectangle it will be clipped. Rectangle format is left, top, width, height.
             *                 If clipArea is set to 0.0f,0.0f,0.0f,0.0f the clipping is turned off.
             */
            void Render(std::string &text, float size, glm::vec2 position, glm::vec4 color, glm::vec4 clipArea = glm::vec4(0.0f));

            void CreateProjectionMatrixForScreen(float width, float height);

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
