//
// Created by gruco0002 on 28.10.2018.
//

#ifndef WORLDENGINE_FONT_HPP
#define WORLDENGINE_FONT_HPP


#include <vector>
#include <libraries/glm/glm.hpp>

#include <map>
#include <string>
#include <engine/graphics/Texture2D.hpp>

namespace Engine {

    namespace Text {

        struct TextVertex {
            glm::vec2 position;
            glm::vec2 textureCoord;
        };

        struct Character {
            unsigned int character;
            glm::vec2 texturePosition;
            glm::vec2 textureSize;
            glm::vec2 size;
            glm::vec2 offset;
            float xAdvance;
        };

        class Font {

        public:
            Font(std::map<unsigned int, Character> characters, Graphics::Texture2D *fontTexture, float lineHeight,
                 float base, glm::vec4 padding, float size);

            Graphics::Texture2D *fontTexture;

            /// Building the vertices needed for a text.
            /// \note Every character consists of 4 vertices (quad) ordered clockwise. The characters position is
            /// in the fonts pixel space. The text starts at 0,0 (top, left) and continues along the positive x and
            /// y axis.
            /// \param text The text which is converted into vertices
            /// \return List of vertices representing the text.
            std::vector<TextVertex> getTextVertices(std::string text, float size = 12);

        private:
            std::map<unsigned int, Character> characters;
            float lineHeight;
            float base;
            float size;
            glm::vec4 padding;

        };

    }

}


#endif //WORLDENGINE_FONT_HPP
