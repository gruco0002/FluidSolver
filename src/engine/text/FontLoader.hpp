//
// Created by gruco0002 on 28.10.2018.
//

#ifndef WORLDENGINE_FONTLOADER_HPP
#define WORLDENGINE_FONTLOADER_HPP

#include <string>
#include "Font.hpp"
#include <libraries/glm/glm.hpp>

namespace Engine {

        namespace Text {

            /// Class for loading distance field fonts from .fnt files.
            class FontLoader {
            public :
                explicit FontLoader(std::string filepath);

                Font *loadToFont();

            private:
                std::vector<Character> characters;
                std::string filepath;
                std::string textureFilename;
                float lineHeight;
                float base;
                float scaleW;
                float scaleH;
                float size;
                glm::vec4 paddings;

                Graphics::Texture2D *texture;

                void processLine(std::string line);

                void processCharLine(std::vector<std::string> lineContents);

                void processCommonLine(std::vector<std::string> lineContents);

                void processPageLine(std::vector<std::string> lineContents);

                void processInfoLine(std::vector<std::string> lineContents);

                void loadTexture();



                std::vector<std::string> split(const std::string &s, char delimiter);

            };

        }

}
#endif //WORLDENGINE_FONTLOADER_HPP
