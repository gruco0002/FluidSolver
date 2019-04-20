//
// Created by gruco0002 on 28.10.2018.
//
#include "Font.hpp"


namespace Engine {

    namespace Text {

        Font::Font(std::map<unsigned int, Character> characters, Graphics::Texture2D *fontTexture, float lineHeight,
                   float base, glm::vec4 padding, float size) {
            this->characters = characters;
            this->fontTexture = fontTexture;
            this->lineHeight = lineHeight;
            this->base = base;
            this->padding = padding;
            this->size = size;
        }

        std::vector<TextVertex> Font::getTextVertices(std::string text, float size) {

            std::vector<TextVertex> vertices;

            float currentX = 0;
            float currentY = 0;
            float sizeFactor = (float) size / (float) this->size;

            for (unsigned int i = 0; i < text.length(); i++) {
                char currentCharTmp = text[i];
                unsigned int currentChar;
                if (currentCharTmp >= 0) {
                    currentChar = currentCharTmp;
                } else {
                    currentChar = 256 + (int) currentCharTmp; // for negative char values
                }

                if (currentChar == '\n') {
                    currentY += lineHeight;
                    currentX = 0;
                }

                if (characters.find(currentChar) == characters.end())
                    continue; // char not in map

                auto character = characters[currentChar];

                float posX = currentX + character.offset.x;
                float posY = currentY + character.offset.y;

                TextVertex v1;
                TextVertex v2;
                TextVertex v3;
                TextVertex v4;

                v1.position.x = posX;
                v1.position.y = posY;
                v1.textureCoord = character.texturePosition;

                v2.position.x = posX + character.size.x;
                v2.position.y = posY;
                v2.textureCoord = character.texturePosition;
                v2.textureCoord.x += character.textureSize.x;

                v3.position.x = posX + character.size.x;
                v3.position.y = posY + character.size.y;
                v3.textureCoord = character.texturePosition;
                v3.textureCoord.x += character.textureSize.x;
                v3.textureCoord.y += character.textureSize.y;

                v4.position.x = posX;
                v4.position.y = posY + character.size.y;
                v4.textureCoord = character.texturePosition;
                v4.textureCoord.y += character.textureSize.y;

                // adding padding
                v1.position += glm::vec2(-padding.x, -padding.y);
                v2.position += glm::vec2(padding.z, -padding.y);
                v3.position += glm::vec2(padding.z, padding.w);
                v4.position += glm::vec2(-padding.x, padding.w);

                // shrinking to size
                v1.position *= sizeFactor;
                v2.position *= sizeFactor;
                v3.position *= sizeFactor;
                v4.position *= sizeFactor;

                vertices.push_back(v1);
                vertices.push_back(v2);
                vertices.push_back(v3);
                vertices.push_back(v4);

                currentX += character.xAdvance;

            }

            return vertices;
        }
    }


}