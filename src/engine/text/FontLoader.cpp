//
// Created by gruco0002 on 28.10.2018.
//


#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>

#include "FontLoader.hpp"

namespace Engine {

    namespace Text {

        FontLoader::FontLoader(std::string filepath) {
            this->filepath = filepath;
        }

        Font *FontLoader::loadToFont() {

            std::ifstream infile(filepath);
            std::string line;
            while (std::getline(infile, line)) {
                processLine(line);
            }

            loadTexture();

            std::map<unsigned int, Character> characterMap;

            // going to texture space and adding into map
            for (unsigned int i = 0; i < characters.size(); i++) {
                Character &character = characters.at(i);
                character.texturePosition.x /= (float) scaleW;
                character.texturePosition.y /= (float) scaleH;
                character.textureSize.x /= (float) scaleW;
                character.textureSize.y /= (float) scaleH;

                characterMap[character.character] = character;
            }

            return new Font(characterMap, texture, lineHeight, base, paddings, size);
        }

        void FontLoader::processLine(std::string line) {
            auto lineContents = split(line, ' ');
            if (lineContents.size() == 0) return;

            if (lineContents.at(0) == "page") {
                processPageLine(lineContents);
            } else if (lineContents.at(0) == "char") {
                processCharLine(lineContents);
            } else if (lineContents.at(0) == "common") {
                processCommonLine(lineContents);
            } else if (lineContents.at(0) == "info") {
                processInfoLine(lineContents);
            }

        }

        std::vector<std::string> FontLoader::split(const std::string &s, char delimiter) {
            std::vector<std::string> tokens;
            std::string token;
            std::istringstream tokenStream(s);
            while (std::getline(tokenStream, token, delimiter)) {
                if (token.length() > 0)
                    tokens.push_back(token);
            }
            return tokens;

        }

        void FontLoader::processCharLine(std::vector<std::string> lineContents) {

            Character character;

            for (unsigned int i = 0; i < lineContents.size(); ++i) {

                std::string content = lineContents.at(i);
                if (content.rfind("id=", 0) == 0) {
                    auto value = std::stoi(content.erase(0, 3));
                    character.character = value;
                }
                if (content.rfind("x=", 0) == 0) {
                    auto value = std::stof(content.erase(0, 2));
                    character.texturePosition.x = value;
                }
                if (content.rfind("y=", 0) == 0) {
                    auto value = std::stof(content.erase(0, 2));
                    character.texturePosition.y = value;
                }
                if (content.rfind("width=", 0) == 0) {
                    auto value = std::stof(content.erase(0, 6));
                    character.textureSize.x = value;
                    character.size.x = value;
                }
                if (content.rfind("height=", 0) == 0) {
                    auto value = std::stof(content.erase(0, 7));
                    character.textureSize.y = value;
                    character.size.y = value;
                }
                if (content.rfind("xoffset=", 0) == 0) {
                    auto value = std::stof(content.erase(0, 8));
                    character.offset.x = value;
                }
                if (content.rfind("yoffset=", 0) == 0) {
                    auto value = std::stof(content.erase(0, 8));
                    character.offset.y = value;
                }
                if (content.rfind("xadvance=", 0) == 0) {
                    auto value = std::stof(content.erase(0, 9));
                    character.xAdvance = value;
                }

            }

            characters.push_back(character);

        }

        void FontLoader::processCommonLine(std::vector<std::string> lineContents) {
            for (unsigned int i = 0; i < lineContents.size(); ++i) {

                std::string content = lineContents.at(i);
                if (content.rfind("lineHeight=", 0) == 0) {
                    auto value = std::stoi(content.erase(0, 11));
                    lineHeight = value;
                }
                if (content.rfind("base=", 0) == 0) {
                    auto value = std::stoi(content.erase(0, 5));
                    base = value;
                }
                if (content.rfind("scaleW=", 0) == 0) {
                    auto value = std::stoi(content.erase(0, 7));
                    scaleW = value;
                }
                if (content.rfind("scaleH=", 0) == 0) {
                    auto value = std::stoi(content.erase(0, 7));
                    scaleH = value;
                }


            }
        }

        void FontLoader::processPageLine(std::vector<std::string> lineContents) {
            for (unsigned int i = 0; i < lineContents.size(); ++i) {
                std::string content = lineContents.at(i);
                if (content.rfind("file=\"", 0) == 0) {
                    auto value = content.erase(content.length() - 1, 1).erase(0, 6);
                    textureFilename = value;
                }
            }
        }

        void FontLoader::loadTexture() {
            // retrieving texture filepath
            unsigned int last = filepath.find_last_of("/\\");
            last += 1;
            std::string directory = filepath;
            directory.erase(last);

            std::string textureFilepath = directory.append(textureFilename);

            // generate texture
            auto textureSettings = new Graphics::Texture2DSettings();
            textureSettings->GenerateMipmaps = false;

            texture = new Graphics::Texture2D(textureFilepath, textureSettings);

        }

        void FontLoader::processInfoLine(std::vector<std::string> lineContents) {
            for (unsigned int i = 0; i < lineContents.size(); ++i) {
                std::string content = lineContents.at(i);
                if (content.rfind("size=", 0) == 0) {
                    auto value = std::stoi(content.erase(0, 5));
                    size = value;
                }
                if (content.rfind("padding=", 0) == 0) {
                    auto value = content.erase(0, 8);
                    std::vector<std::string> paddingValues = split(value, ',');
                    paddings = glm::vec4(std::stof(paddingValues[3]), std::stof(paddingValues[0]),
                                         std::stof(paddingValues[1]), std::stof(paddingValues[2]));
                }
            }
        }


    }


}