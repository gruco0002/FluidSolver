//
// Created by corbi on 20.04.2019.
//

#include <engine/EngineException.hpp>
#include "TextRenderer.hpp"
#include <libraries/glm/gtc/matrix_transform.hpp>
#include <algorithm>

Engine::Text::TextRenderer::TextRenderer(Engine::Text::Font *font) {
    this->font = font;
    Generate();
}

void Engine::Text::TextRenderer::Render(std::string &text, float size, glm::vec2 position, glm::vec4 color,
                                        glm::vec4 clipArea) {

    if (text.size() > 10922) throw EngineException("Text Renderer Error: Max length of 10922 characters exceeded");

    auto data = font->getTextVertices(text, size);
    vertexBuffer->UpdateData(data);

    auto offset = GetVertexNegative(data); // needed because text goes into negative

    textShader->Bind();
    textShader->SetValue("aLocation", position - offset);
    textShader->SetValue("fontTexture", font->fontTexture, GL_TEXTURE0);
    textShader->SetValue("textColor", color);
    textShader->SetValue("distanceFieldWidth", distanceFieldWidth);
    textShader->SetValue("distanceFieldEdge", distanceFieldEdge);
    textShader->SetValue("projectionMatrix", projectionMatrix);
    if (clipArea == glm::vec4(0.0f)) {
        textShader->SetValue("clippingEnabled", false);
    } else {
        textShader->SetValue("clippingEnabled", true);
        textShader->SetValue("clippingRectangle", clipArea);
    }



    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    vertexArray->Draw(GL_TRIANGLES, data.size() * 6 / 4); // 6 indices per character, but 4 data-vertex per character
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
uniform mat4 projectionMatrix;

out vec2 TexCoord;
out vec2 PosCoord;

void main()
{
    PosCoord = aPos + aLocation;
    gl_Position = projectionMatrix* vec4(PosCoord, 0.0, 1.0);
    TexCoord = aTex;
}
)";

const std::string frag = R"(#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec2 PosCoord;

uniform sampler2D fontTexture;
uniform vec4 textColor;

uniform float distanceFieldWidth;
uniform float distanceFieldEdge;

uniform vec4 clippingRectangle;
uniform bool clippingEnabled;

void main()
{
    if(clippingEnabled == true){

        float left = clippingRectangle.x;
        float top = clippingRectangle.y;
        float width = clippingRectangle.z;
        float height = clippingRectangle.w;

        if(PosCoord.x < left || PosCoord.y < top || PosCoord.x > left + width || PosCoord.y > top + height){
            discard;
        }

    }

    float distance = 1.0 -  texture(fontTexture, TexCoord).a;
    float alpha = 1.0 - smoothstep(distanceFieldWidth, distanceFieldWidth + distanceFieldEdge, distance);
    //alpha = max(alpha, 0.5);
    FragColor = vec4(textColor.rgb, alpha * textColor.a);
    //FragColor = mix(FragColor, vec4(1.0,0.0,0.0,1.0), 0.5);
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

void Engine::Text::TextRenderer::CreateProjectionMatrixForScreen(float width, float height) {
    projectionMatrix = glm::ortho(0.0f, (float) width, (float) height, 0.0f);
}

glm::vec2 Engine::Text::TextRenderer::GetTextDimensions(std::string &text, float size) {
    if (text == "")return glm::vec2(0.0f);
    auto vert = font->getTextVertices(text, size);
    float minX, minY, maxX, maxY;
    minX = vert[0].position.x;
    maxX = vert[0].position.x;
    minY = vert[0].position.y;
    maxY = vert[0].position.y;
    for (auto v : vert) {
        minX = std::min(minX, v.position.x);
        minY = std::min(minY, v.position.y);
        maxX = std::max(maxX, v.position.x);
        maxY = std::max(maxY, v.position.y);
    }
    return glm::vec2(maxX - minX, maxY - minY);
}

glm::vec2 Engine::Text::TextRenderer::GetVertexNegative(std::vector<Engine::Text::TextVertex> &vertices) {
    if (vertices.size() == 0)
        return glm::vec2(0.0f);
    float minX, minY;
    // float maxX, maxY;
    minX = vertices[0].position.x;
    // maxX = vertices[0].position.x;
    minY = vertices[0].position.y;
    // maxY = vertices[0].position.y;
    for (auto v : vertices) {
        minX = std::min(minX, v.position.x);
        minY = std::min(minY, v.position.y);
        //  maxX = std::max(maxX, v.position.x);
        //  maxY = std::max(maxY, v.position.y);
    }
    return glm::vec2(minX, minY);
}

