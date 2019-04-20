//
// Created by corbi on 20.04.2019.
//

#include <engine/EngineException.hpp>
#include "TextRenderer.hpp"

Engine::Text::TextRenderer::TextRenderer(Engine::Text::Font *font) {
    this->font = font;
    Generate();
}

void Engine::Text::TextRenderer::Render(std::string &text, float size, glm::vec2 position, glm::vec4 color) {

    if (text.size() > 10922) throw EngineException("Text Renderer Error: Max length of 10922 characters exceeded");

    textShader->Bind();
    textShader->SetValue("aLocation", position);
    textShader->SetValue("fontTexture", font->fontTexture, GL_TEXTURE0);
    textShader->SetValue("textColor", color);
    textShader->SetValue("distanceFieldWidth", distanceFieldWidth);
    textShader->SetValue("distanceFieldEdge", distanceFieldEdge);
	
	auto data = font->getTextVertices(text, size);
    vertexBuffer->UpdateData(data);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    vertexArray->Draw(GL_TRIANGLES, 0, data.size() * 6 / 4);
}

Engine::Text::TextRenderer::~TextRenderer() {
    Delete();
}

void Engine::Text::TextRenderer::Generate() {

    GenerateShader();

    vertexBuffer = new Graphics::Buffer::VertexBuffer<TextVertex>(43688,
                                                                  Graphics::Buffer::Buffer::DataModeDynamic);


    std::vector<uint16_t> indices;
    for (uint32_t t = 0; t <= (UINT16_MAX - 3) / 6; t++) {
        indices.push_back(t * 4 + 0); // top right triangle of character
        indices.push_back(t * 4 + 1);
        indices.push_back(t * 4 + 2);

        indices.push_back(t * 4 + 0); // bottom left triangle of character
        indices.push_back(t * 4 + 2);
        indices.push_back(t * 4 + 3);
        // resulting in max draw count of (2^16 - 1) - 3 = 65532 indices which are 10922 characters which are 43688 character vertices
    }

    indexBuffer = new Graphics::Buffer::IndexBuffer<uint16_t>(indices, Graphics::Buffer::Buffer::DataModeStatic);

    vertexArray = new Graphics::Buffer::VertexArray({
                                                            Graphics::Buffer::VertexArray::BufferBinding(
                                                                    vertexBuffer,
                                                                    0, 2, offsetof(TextVertex, position),
                                                                    sizeof(TextVertex), ComponentTypeFloat

                                                            ),
                                                            Graphics::Buffer::VertexArray::BufferBinding(
                                                                    vertexBuffer,
                                                                    1, 2, offsetof(TextVertex, textureCoord),
                                                                    sizeof(TextVertex), ComponentTypeFloat

                                                            ),
                                                            Graphics::Buffer::VertexArray::BufferBinding(indexBuffer),
                                                    });


}

void Engine::Text::TextRenderer::Delete() {
    delete textShader;
    delete vertexArray;
    delete vertexBuffer;
}

const std::string vert = R"(#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTex;

uniform vec2 aLocation;
uniform mat4 projection;

out vec2 TexCoord;

void main()
{
    gl_Position = projection * vec4(aPos + aLocation, 0.0, 1.0);
    TexCoord = aTex;
}
)";

const std::string frag = R"(#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D fontTexture;
uniform vec4 textColor;

uniform float distanceFieldWidth;
uniform float distanceFieldEdge;

void main()
{
    float distance = 1.0 -  texture(fontTexture, TexCoord).a;
    float alpha = 1.0 - smoothstep(distanceFieldWidth, distanceFieldWidth + distanceFieldEdge, distance);
    //alpha = max(alpha, 0.5);
    FragColor = vec4(1.0, 1.0, textColor.r, alpha);

}

)";

void Engine::Text::TextRenderer::GenerateShader() {


    textShader = new Graphics::Shader({
                                              Graphics::Shader::ProgramPart(Graphics::Shader::ProgramPartTypeVertex,
                                                                            vert),
                                              Graphics::Shader::ProgramPart(Graphics::Shader::ProgramPartTypeFragment,
                                                                            frag),
                                      });
}

