#include "Font.hpp"

#include "stb_truetype.h"

#include <cstring>
#include <fstream>


namespace Engine
{

    namespace Text
    {

        std::vector<char> getFileContent(std::ifstream& stream)
        {

            stream.seekg(0, stream.end);
            auto size = stream.tellg();
            stream.clear();
            stream.seekg(0, stream.beg);

            std::vector<char> buffer(size);

            stream.read(buffer.data(), size);

            return buffer;
        }

        Font::Font(std::string filename, float pixelSize, int32_t padding, uint8_t edgeValue) : edgeValue(edgeValue)
        {

            stbtt_fontinfo font;
            std::string fontString;

            std::ifstream fontFile(filename, std::ios::binary);

            if (!fontFile.is_open())
            {
                throw std::invalid_argument("Couldn't open font file");
            }

            auto buffer = getFileContent(fontFile);

            fontFile.close();

            if (!stbtt_InitFont(&font, (unsigned char*)buffer.data(), 0))
            {
                throw std::invalid_argument("Failed loading font");
            }

            glyphs.resize(ENGINE_FONT_GLYPH_COUNT);
            glyphInfo.resize(ENGINE_FONT_GLYPH_COUNT);

            firstGlyphBuffer = new Graphics::Buffer::UniformBuffer<GlyphInfo>(ENGINE_GPU_GLYPH_COUNT / 2);

            secondGlyphBuffer = new Graphics::Buffer::UniformBuffer<GlyphInfo>(ENGINE_GPU_GLYPH_COUNT / 2);

            float scale = (float)stbtt_ScaleForPixelHeight(&font, pixelSize);

            int32_t iAscent, iDescent, iLineGap;
            stbtt_GetFontVMetrics(&font, &iAscent, &iDescent, &iLineGap);

            ascent = (float)iAscent * scale;
            descent = (float)iDescent * scale;
            lineGap = (float)iLineGap * scale;

            lineHeight = ascent - descent + lineGap;

            auto pixelDistanceScale = (float)edgeValue / (float)padding;

            int32_t codepointCount = 0;
            int32_t glyphCount = 0;
            // Check the number of codepoints for the total number of glyphs
            while (glyphCount < font.numGlyphs && codepointCount < ENGINE_FONT_GLYPH_COUNT)
            {
                // Larger zero if there exists a corresponding glyph to the codepoint
                if (stbtt_FindGlyphIndex(&font, codepointCount))
                    glyphCount++;
                codepointCount++;
            }

            codepointCount = codepointCount > ENGINE_FONT_GLYPH_COUNT ? ENGINE_FONT_GLYPH_COUNT : codepointCount;

            uint8_t* data[ENGINE_FONT_GLYPH_COUNT];

            int32_t width = 0, height = 0, depth = 0;
            glm::ivec2 offset = glm::ivec2(0, 0);

            // Initialize the kern for every glyph possible
            for (int32_t i = 0; i < ENGINE_FONT_GLYPH_COUNT; i++)
                glyphs[i].kern.resize(ENGINE_FONT_GLYPH_COUNT);

            // Load the data and calculate the needed resolution for the texture
            for (int32_t i = font.fontstart; i < codepointCount; i++)
            {

                auto glyph = &glyphs[i];

                glyph->codepoint = i;

                data[i] = stbtt_GetCodepointSDF(&font, scale, i, padding, edgeValue, pixelDistanceScale, &glyph->width,
                                                &glyph->height, &offset.x, &offset.y);

                for (int32_t j = 0; j < ENGINE_FONT_GLYPH_COUNT; j++)
                {
                    glyph->kern[j] = (int32_t)((float)stbtt_GetCodepointKernAdvance(&font, i, j) * scale);
                }

                stbtt_GetCodepointHMetrics(&font, i, &glyph->advance, 0);
                glyph->advance = (int32_t)((float)glyph->advance * scale);

                if (data[i] == nullptr)
                {
                    glyph->height = 0;
                    glyph->width = 0;
                    glyph->offset = glm::vec2(0.0f);
                    glyph->textureScale = glm::vec2(0.0f);
                    glyph->texArrayIndex = ENGINE_GPU_GLYPH_COUNT;
                    continue;
                }

                glyph->offset.x = (float)offset.x;
                glyph->offset.y = (float)offset.y;

                width = glyph->width > width ? glyph->width : width;
                height = glyph->height > height ? glyph->height : height;
                glyph->texArrayIndex = depth++;
            }

            depth = ENGINE_GPU_GLYPH_COUNT < depth ? ENGINE_GPU_GLYPH_COUNT : depth;

            // Create texture and process texture data
            auto settings = new Graphics::Texture2DSettings();
            settings->GenerateMipmaps = false;
            settings->TextureMagnifyingFiltering = GL_LINEAR;
            settings->TextureMinifyingFiltering = GL_LINEAR;
            settings->TextureWrapping = GL_CLAMP_TO_EDGE;
            glyphTexture =
                new Graphics::Texture2DArray(width, height, depth, settings, GL_RED, GL_R8, ComponentTypeUnsignedByte);

            std::vector<uint8_t> glyphData(width * height);

            for (int32_t i = font.fontstart; i < codepointCount; i++)
            {

                if (data[i] == nullptr)
                    continue;

                auto glyph = &glyphs[i];

                std::memset(glyphData.data(), 0, glyphData.size());

                glyph->textureScale.x = (float)glyph->width / (float)width;
                glyph->textureScale.y = (float)glyph->height / (float)height;

                // Hard coded end. All other symbols aren't supported for now.
                // This still supports all the latin characters of UTF-8
                if (glyph->texArrayIndex < depth)
                {

                    for (int32_t x = 0; x < glyph->width; x++)
                    {
                        for (int32_t y = 0; y < glyph->height; y++)
                        {
                            glyphData[y * width + x] = data[i][y * glyph->width + x];
                        }
                    }

                    glyphInfo[glyph->texArrayIndex].scale = glyph->textureScale;
                    glyphInfo[glyph->texArrayIndex].size = glm::vec2((float)glyph->width, (float)glyph->height);

                    glyphTexture->SetData(glyphData, glyph->texArrayIndex);
                }
                else
                {

                    glyph->texArrayIndex = ENGINE_GPU_GLYPH_COUNT;
                }

                delete[] data[i];
            }

            firstGlyphBuffer->SetData(&glyphInfo[0], ENGINE_GPU_GLYPH_COUNT / 2);
            secondGlyphBuffer->SetData(&glyphInfo[ENGINE_GPU_GLYPH_COUNT / 2], ENGINE_GPU_GLYPH_COUNT / 2);
        }

        Glyph* Font::GetGlyph(char character)
        {

            uint8_t characterIndex = (uint8_t)character;

            if (characterIndex > ENGINE_FONT_GLYPH_COUNT)
                return nullptr;

            return &glyphs[characterIndex];
        }

        Glyph* Font::GetGlyphUTF8(const char*& character)
        {

            uint8_t byte = *character;
            uint32_t unicode = 0;

            if (!(byte & 0x80))
            {
                unicode = (uint32_t)byte;
                character++;
            }
            else if (((byte & 0xc0) == 0xc0) && !(byte & 0x20))
            {
                unicode = ((byte & 0x1f) << 6);
                character++;
                byte = *character;
                unicode |= (byte & 0x3f);
                character++;
            }
            else if (((byte & 0xe0) == 0xe0) && !(byte & 0x10))
            {
                character += 3;
            }
            else
            {
                character += 4;
            }

            if (unicode > ENGINE_FONT_GLYPH_COUNT)
                return nullptr;

            return &glyphs[unicode];
        }

        void Font::ComputeDimensions(std::string text, float scale, float* width, float* height)
        {

            *width = 0;
            *height = lineHeight * scale;

            auto ctext = text.c_str();

            auto nextGlyph = GetGlyphUTF8(ctext);

            while (nextGlyph->codepoint)
            {

                auto glyph = nextGlyph;
                nextGlyph = GetGlyphUTF8(ctext);

                *width += ((float)(glyph->advance + glyph->kern[nextGlyph->codepoint]) * scale);
            }
        }

        Font::~Font()
        {
            delete firstGlyphBuffer;
            delete secondGlyphBuffer;
            delete glyphTexture;
        }

        Glyph* Font::GetGlyphByCodepoint(uint32_t codepoint)
        {
            if (glyphs.size() <= codepoint)
                return nullptr;
            return &glyphs[codepoint];
        }


    } // namespace Text


} // namespace Engine