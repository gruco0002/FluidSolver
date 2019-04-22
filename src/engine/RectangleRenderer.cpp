//
// Created by corbi on 22.04.2019.
//

#include "RectangleRenderer.hpp"
#include <libraries/glm/gtc/matrix_transform.hpp>

void Engine::RectangleRenderer::RenderRectangle(glm::vec2 position, glm::vec2 size, glm::vec4 color,
                                                glm::vec4 clipArea) {
    UpdateVertices(position, size);
    simpleRectangleShader->Bind();
    simpleRectangleShader->SetValue("rectColor", color);
    simpleRectangleShader->SetValue("projectionMatrix", projectionMatrix);
    if (clipArea == glm::vec4(0.0f)) {
        simpleRectangleShader->SetValue("clippingEnabled", false);
    } else {
        simpleRectangleShader->SetValue("clippingEnabled", true);
        simpleRectangleShader->SetValue("clippingRectangle", clipArea);
    }

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    vertexArray->Draw(GL_TRIANGLES, 6);

}

void Engine::RectangleRenderer::RenderTexture(glm::vec2 position, glm::vec2 size, Engine::Graphics::Texture2D *texture,
                                              glm::vec4 clipArea) {
    UpdateVertices(position, size);
    textureRectangleShader->Bind();
    textureRectangleShader->SetValue("rectTexture", texture, GL_TEXTURE0);
    textureRectangleShader->SetValue("projectionMatrix", projectionMatrix);
    if (clipArea == glm::vec4(0.0f)) {
        textureRectangleShader->SetValue("clippingEnabled", false);
    } else {
        textureRectangleShader->SetValue("clippingEnabled", true);
        textureRectangleShader->SetValue("clippingRectangle", clipArea);
    }

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    vertexArray->Draw(GL_TRIANGLES, 6);
}

void Engine::RectangleRenderer::CreateProjectionMatrixForScreen(float width, float height) {
    projectionMatrix = glm::ortho(0.0f, (float) width, (float) height, 0.0f);
}

Engine::RectangleRenderer::~RectangleRenderer() {
    Delete();
}

void Engine::RectangleRenderer::Generate() {
    GenerateShaders();

    vertexBuffer = new Engine::Graphics::Buffer::VertexBuffer<glm::vec2>(4,
                                                                         Engine::Graphics::Buffer::Buffer::DataModeDynamic);

    auto texCoord = std::vector<glm::vec2>{
            glm::vec2(0.0, 1.0), // top left
            glm::vec2(1.0, 1.0), // top right
            glm::vec2(1.0, 0.0), // bottom right
            glm::vec2(0.0, 0.0), // bottom left
    };
    textureCoordBuffer = new Engine::Graphics::Buffer::VertexBuffer<glm::vec2>(texCoord);

    auto indices = std::vector<uint8_t>{
            0, 1, 2, 0, 2, 3
    };
    indexBuffer = new Engine::Graphics::Buffer::IndexBuffer<uint8_t>(indices);
    vertexArray = new Engine::Graphics::Buffer::VertexArray({
                                                                    Engine::Graphics::Buffer::VertexArray::BufferBinding(
                                                                            vertexBuffer, 0, 2, offsetof(glm::vec2, x),
                                                                            sizeof(glm::vec2), ComponentTypeFloat
                                                                    ),
                                                                    Engine::Graphics::Buffer::VertexArray::BufferBinding(
                                                                            textureCoordBuffer, 1, 2,
                                                                            offsetof(glm::vec2, x), sizeof(glm::vec2),
                                                                            ComponentTypeFloat
                                                                    ),
                                                                    Engine::Graphics::Buffer::VertexArray::BufferBinding(
                                                                            indexBuffer),
                                                            });
}

void Engine::RectangleRenderer::Delete() {
    delete vertexArray;
    delete vertexBuffer;
    delete textureCoordBuffer;
    delete simpleRectangleShader;
    delete textureRectangleShader;
}

const std::string shaderVert = R"(#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTex;

uniform mat4 projectionMatrix;

out vec2 TexCoord;
out vec2 PosCoord;

void main()
{
    PosCoord = aPos;
    gl_Position = projectionMatrix* vec4(PosCoord, 0.0, 1.0);
    TexCoord = aTex;
}
)";

const std::string shaderFrag = R"(#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec2 PosCoord;

#ifdef TEXTURE_USED
    uniform sampler2D rectTexture;
#else
    uniform vec4 rectColor;
#endif

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

    #ifdef TEXTURE_USED
        FragColor = texture(rectTexture, TexCoord);
    #else
        FragColor = rectColor;
    #endif

}

)";

void Engine::RectangleRenderer::GenerateShaders() {

    simpleRectangleShader = new Engine::Graphics::Shader({
                                                                 Engine::Graphics::Shader::ProgramPart(
                                                                         Engine::Graphics::Shader::ProgramPartTypeVertex,
                                                                         shaderVert),
                                                                 Engine::Graphics::Shader::ProgramPart(
                                                                         Engine::Graphics::Shader::ProgramPartTypeFragment,
                                                                         shaderFrag),
                                                         });
    textureRectangleShader = new Engine::Graphics::Shader(
            {Engine::Graphics::Shader::ProgramPart(Engine::Graphics::Shader::ProgramPartTypeVertex, shaderVert),
             Engine::Graphics::Shader::ProgramPart(Engine::Graphics::Shader::ProgramPartTypeFragment, shaderFrag),},
            {
                    "TEXTURE_USED"
            });

}

Engine::RectangleRenderer::RectangleRenderer() {
    Generate();
}

void Engine::RectangleRenderer::UpdateVertices(glm::vec2 position, glm::vec2 size) {
    auto vert = std::vector<glm::vec2>{
            position,
            position + glm::vec2(size.x, 0.0f),
            position + size,
            position + glm::vec2(0.0f, size.y),
    };
    vertexBuffer->UpdateData(vert);
}
