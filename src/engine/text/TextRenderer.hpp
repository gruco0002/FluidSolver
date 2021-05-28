#ifndef ENGINE_TEXTRENDERER_HPP
#define ENGINE_TEXTRENDERER_HPP

#include "../graphics/Shader.hpp"
#include "../graphics/buffer/IndexBuffer.hpp"
#include "../graphics/buffer/VertexArray.hpp"
#include "../graphics/buffer/VertexBuffer.hpp"
#include "Font.hpp"

namespace Engine
{
    namespace Text
    {
        class TextRenderer {
          public:
            TextRenderer(Font* font);

            void Render(std::string text, float x, float y, float size, glm::vec4 color = glm::vec4(1.0f));

            void Render(std::string text, float x, float y, float size, glm::vec4 color, glm::vec4 clipArea,
                        glm::vec4 blendArea);

            void RenderOutlined(std::string text, float x, float y, float size, glm::vec4 color, glm::vec4 outlineColor,
                                float outlineScale);

            void RenderOutlined(std::string text, float x, float y, float size, glm::vec4 color, glm::vec4 outlineColor,
                                float outlineScale, glm::vec4 clipArea, glm::vec4 blendArea);

            void RenderCodepoint(uint32_t codepoint, float x, float y, float size, glm::vec4 color, glm::vec4 clipArea,
                                 glm::vec4 blendArea);


            void RenderOutlinedCodepoint(uint32_t codepoint, float x, float y, float size, glm::vec4 color,
                                         glm::vec4 outlineColor, float outlineScale, glm::vec4 clipArea,
                                         glm::vec4 blendArea);


            glm::mat4 projectionMatrix;

            Font* font = nullptr;

            void CreateProjectionMatrixForScreen(float width, float height);

            glm::vec2 GetTextDimensions(std::string& text, float size);

          private:
            std::vector<glm::vec3> CalculateCharacterInstances(std::string text, int32_t* characterCount);
            std::vector<glm::vec3> CalculateCharacterInstances(uint32_t codepoint, int32_t* characterCount);

            Graphics::Buffer::VertexArray* vertexArray = nullptr;

            Graphics::Shader* shader = nullptr;

            Graphics::Buffer::VertexBuffer<float>* rectangleBuffer = nullptr;

            Graphics::Buffer::VertexBuffer<glm::vec3>* vertexBuffer = nullptr;

            Engine::Graphics::Buffer::IndexBuffer<uint8_t>* indexBuffer = nullptr;

            void CreateShader();

            void CreateVertexArray();

          public:
            virtual ~TextRenderer();

          private:
            static const std::string shaderVert;
            static const std::string shaderFrag;
        };
    } // namespace Text
} // namespace Engine

#endif // ENGINE_TEXTRENDERER_HPP
