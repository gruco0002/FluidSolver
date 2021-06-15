#include "TextRenderer.hpp"

#include "../EngineException.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <algorithm>


void Engine::Text::TextRenderer::Render(std::string text, float x, float y, float size, glm::vec4 color)
{
    float width = std::numeric_limits<float>::max();
    float height = std::numeric_limits<float>::max();

    glm::vec4 clipArea = glm::vec4(0.0f, 0.0f, width, height);
    glm::vec4 blendArea = glm::vec4(0.0f, 0.0f, width, height);

    RenderOutlined(text, x, y, size, color, glm::vec4(1.0f), 0.0f, clipArea, blendArea);
}

void Engine::Text::TextRenderer::Render(std::string text, float x, float y, float size, glm::vec4 color,
                                        glm::vec4 clipArea, glm::vec4 blendArea)
{
    RenderOutlined(text, x, y, size, color, glm::vec4(1.0f), 0.0f, clipArea, blendArea);
}

void Engine::Text::TextRenderer::RenderOutlined(std::string text, float x, float y, float size, glm::vec4 color,
                                                glm::vec4 outlineColor, float outlineScale)
{
    float width = std::numeric_limits<float>::max();
    float height = std::numeric_limits<float>::max();

    glm::vec4 clipArea = glm::vec4(0.0f, 0.0f, width, height);
    glm::vec4 blendArea = glm::vec4(0.0f, 0.0f, width, height);

    RenderOutlined(text, x, y, size, color, outlineColor, outlineScale, clipArea, blendArea);
}

void Engine::Text::TextRenderer::RenderOutlined(std::string text, float x, float y, float size, glm::vec4 color,
                                                glm::vec4 outlineColor, float outlineScale, glm::vec4 clipArea,
                                                glm::vec4 blendArea)
{

    float scale = size / (float)(font->lineHeight - font->lineGap);
    int32_t characterCount;

    shader->Bind();

    glDisable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    auto instances = CalculateCharacterInstances(text, &characterCount);
    vertexBuffer->UpdateData(instances);

    shader->SetValue("pMatrix", projectionMatrix);

    shader->SetValue("textScale", scale);
    shader->SetValue("textOffset", glm::vec2(x, y));
    shader->SetValue("textColor", color);

    shader->SetValue("outlineColor", outlineColor);
    shader->SetValue("outlineScale", outlineScale);

    shader->SetValue("edgeValue", ((float)font->edgeValue) / 255.0f);
    shader->SetValue("smoothness", font->smoothness);

    shader->SetValue("clipArea", clipArea);
    // shader->SetValue("blendArea", blendArea); // currently not used in shader

    shader->SetValue("glyphsTexture", font->glyphTexture, GL_TEXTURE0);

    shader->SetValue("UBO1", font->firstGlyphBuffer, 0);
    shader->SetValue("UBO2", font->secondGlyphBuffer, 1);

    vertexArray->Bind();

    // glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, characterCount);
    glDrawElementsInstanced(GL_TRIANGLE_STRIP, 4, indexBuffer->GetIndexBufferDataType(), nullptr, characterCount);

    glDisable(GL_BLEND);

    vertexArray->Unbind();


    font->firstGlyphBuffer->Unbind();
    font->secondGlyphBuffer->Unbind();

    // glEnable(GL_CULL_FACE);
}

void Engine::Text::TextRenderer::RenderOutlinedCodepoint(uint32_t codepoint, float x, float y, float size,
                                                         glm::vec4 color, glm::vec4 outlineColor, float outlineScale,
                                                         glm::vec4 clipArea, glm::vec4 blendArea)
{
    float scale = size / (float)(font->lineHeight - font->lineGap);
    int32_t characterCount;

    shader->Bind();

    glDisable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    auto instances = CalculateCharacterInstances(codepoint, &characterCount);
    vertexBuffer->UpdateData(instances);

    shader->SetValue("pMatrix", projectionMatrix);

    shader->SetValue("textScale", scale);
    shader->SetValue("textOffset", glm::vec2(x, y));
    shader->SetValue("textColor", color);

    shader->SetValue("outlineColor", outlineColor);
    shader->SetValue("outlineScale", outlineScale);

    shader->SetValue("edgeValue", ((float)font->edgeValue) / 255.0f);
    shader->SetValue("smoothness", font->smoothness);

    shader->SetValue("clipArea", clipArea);
    // shader->SetValue("blendArea", blendArea); // currently not used in shader

    shader->SetValue("glyphsTexture", font->glyphTexture, GL_TEXTURE0);

    shader->SetValue("UBO1", font->firstGlyphBuffer, 0);
    shader->SetValue("UBO2", font->secondGlyphBuffer, 1);

    vertexArray->Bind();

    // glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, characterCount);
    glDrawElementsInstanced(GL_TRIANGLE_STRIP, 4, indexBuffer->GetIndexBufferDataType(), nullptr, characterCount);

    glDisable(GL_BLEND);

    vertexArray->Unbind();


    font->firstGlyphBuffer->Unbind();
    font->secondGlyphBuffer->Unbind();

    // glEnable(GL_CULL_FACE);
}


std::vector<glm::vec3> Engine::Text::TextRenderer::CalculateCharacterInstances(std::string text,
                                                                               int32_t* characterCount)
{
    *characterCount = 0;

    auto instances = std::vector<glm::vec3>(text.length());

    int32_t index = 0;

    float xOffset = 0.0f;

    auto ctext = text.c_str();

    auto nextGlyph = font->GetGlyphUTF8(ctext);

    while (nextGlyph->codepoint)
    {

        Glyph* glyph = nextGlyph;

        // Just visible characters should be rendered.
        if (glyph->codepoint > 32 && glyph->texArrayIndex < ENGINE_GPU_GLYPH_COUNT)
        {
            instances[index].x = glyph->offset.x + xOffset;
            instances[index].y = glyph->offset.y + font->ascent;
            instances[index].z = (float)glyph->texArrayIndex;
            index++;
        }

        nextGlyph = font->GetGlyphUTF8(ctext);

        xOffset += glyph->advance + glyph->kern[nextGlyph->codepoint];
    }

    *characterCount = index;

    return instances;
}

Engine::Text::TextRenderer::TextRenderer(Engine::Text::Font* font) : font(font)
{
    CreateShader();
    CreateVertexArray();
}

void Engine::Text::TextRenderer::CreateProjectionMatrixForScreen(float width, float height)
{
    projectionMatrix = glm::ortho(0.0f, (float)width, (float)height, 0.0f);
}


const std::string Engine::Text::TextRenderer::shaderVert = R"(#version 410 core

layout(location=0)in vec2 vPosition;
layout(location=1)in vec3 characterInfo;

out vec3 fTexCoord;
out vec2 fScreenPosition;

uniform mat4 pMatrix;

uniform vec2 textOffset;
uniform float textScale;

struct GlyphInfo {
	vec2 scale;
	vec2 size;
};

layout (std140) uniform UBO1 {
    GlyphInfo glyphs1[1024];
};

layout (std140) uniform UBO2 {
    GlyphInfo glyphs2[1024];
};

void main() {

	int glyphIndex = int(characterInfo.z);

	GlyphInfo glyph = glyphIndex > 1023 ? glyphs2[glyphIndex - 1024] : glyphs1[glyphIndex];

	vec2 position = (vPosition + 1.0) / 2.0;
	fScreenPosition = position * glyph.size * textScale + characterInfo.xy * textScale + textOffset;
    gl_Position = pMatrix * vec4(fScreenPosition, 0.0, 1.0);
	//gl_Position.y *= -1.0f;
    fTexCoord = vec3(position * glyph.scale, characterInfo.z);

}
)";
const std::string Engine::Text::TextRenderer::shaderFrag = R"(#version 410 core

layout (location = 0) out vec4 color;

in vec3 fTexCoord;
in vec2 fScreenPosition;

uniform sampler2DArray glyphsTexture;
uniform vec4 textColor;
uniform vec4 outlineColor;
uniform float outlineScale;

uniform float edgeValue;
uniform float smoothness;

uniform vec4 clipArea;
uniform vec4 blendArea;

uniform float textScale;

void main() {

	color = vec4(0.0);

    if (fScreenPosition.x < clipArea.x ||
		fScreenPosition.y < clipArea.y ||
        fScreenPosition.x > clipArea.x + clipArea.z ||
        fScreenPosition.y > clipArea.y + clipArea.w)
            discard;

	float intensity = texture(glyphsTexture, fTexCoord).r;
	float smoothing = smoothness * fwidth(intensity);

	float outlineFactor = smoothstep(edgeValue - smoothing, edgeValue + smoothing, intensity);
	color = mix(outlineColor, textColor, outlineFactor);

	float mixDistance = mix(edgeValue, 0.0, outlineScale);
	float alpha = smoothstep(mixDistance - smoothing, mixDistance + smoothing, intensity);

	color = outlineScale > 0.0 ? vec4(color.rgb, color.a * alpha) :
		vec4(textColor.rgb, textColor.a * alpha);

}
)";

void Engine::Text::TextRenderer::CreateShader()
{
    shader = new Engine::Graphics::Shader({
        Engine::Graphics::Shader::ProgramPart(Engine::Graphics::Shader::ProgramPartTypeVertex, shaderVert),
        Engine::Graphics::Shader::ProgramPart(Engine::Graphics::Shader::ProgramPartTypeFragment, shaderFrag),
    });
}

void Engine::Text::TextRenderer::CreateVertexArray()
{
    std::vector<float> rect({-1, -1, 1, -1, -1, 1, 1, 1});
    rectangleBuffer = new Graphics::Buffer::VertexBuffer<float>(rect);
    vertexBuffer = new Graphics::Buffer::VertexBuffer<glm::vec3>(5000, Graphics::Buffer::Buffer::DataModeStream);

    auto indices = std::vector<uint8_t>{0, 1, 2, 3};
    indexBuffer = new Engine::Graphics::Buffer::IndexBuffer<uint8_t>(indices);


    Graphics::Buffer::VertexArray::BufferBinding rectangleBufferBinding(rectangleBuffer, 0, 2, 0, sizeof(float) * 2,
                                                                        ComponentTypeFloat);

    Graphics::Buffer::VertexArray::BufferBinding vertexBufferBinding(vertexBuffer, 1, 3, 0, sizeof(glm::vec3),
                                                                     ComponentTypeFloat, true);

    std::vector<Graphics::Buffer::VertexArray::BufferBinding> bindings({
        rectangleBufferBinding,
        vertexBufferBinding,
        Engine::Graphics::Buffer::VertexArray::BufferBinding(indexBuffer),
    });
    vertexArray = new Graphics::Buffer::VertexArray(bindings);
}

glm::vec2 Engine::Text::TextRenderer::GetTextDimensions(std::string& text, float size)
{
    float width;
    float height;
    float scale = size / (float)(font->lineHeight - font->lineGap);
    font->ComputeDimensions(text, scale, &width, &height);
    return glm::vec2(width, height);
}

Engine::Text::TextRenderer::~TextRenderer()
{
    delete vertexArray;
    delete vertexBuffer;
    delete rectangleBuffer;
    delete indexBuffer;
    delete shader;
}

std::vector<glm::vec3> Engine::Text::TextRenderer::CalculateCharacterInstances(uint32_t codepoint,
                                                                               int32_t* characterCount)
{
    *characterCount = 0;

    auto instances = std::vector<glm::vec3>(1);

    int32_t index = 0;

    float xOffset = 0.0f;


    Glyph* glyph = font->GetGlyphByCodepoint(codepoint);
    if (glyph == nullptr)
        return std::vector<glm::vec3>();

    // Just visible characters should be rendered.
    if (glyph->codepoint > 32 && glyph->texArrayIndex < ENGINE_GPU_GLYPH_COUNT)
    {
        instances[index].x = glyph->offset.x + xOffset;
        instances[index].y = glyph->offset.y + font->ascent;
        instances[index].z = (float)glyph->texArrayIndex;
        index++;
    }


    //  xOffset += glyph->advance + glyph->kern[nextGlyph->codepoint];


    *characterCount = index;

    return instances;
}

void Engine::Text::TextRenderer::RenderCodepoint(uint32_t codepoint, float x, float y, float size, glm::vec4 color,
                                                 glm::vec4 clipArea, glm::vec4 blendArea)
{
    RenderOutlinedCodepoint(codepoint, x, y, size, color, glm::vec4(1.0f), 0.0f, clipArea, blendArea);
}
