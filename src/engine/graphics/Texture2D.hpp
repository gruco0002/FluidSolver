//
// Created by corbi on 17.04.2019.
//

#ifndef FLUIDSOLVER_TEXTURE2D_HPP
#define FLUIDSOLVER_TEXTURE2D_HPP


#include <string>
#include <vector>
#include <glad/glad.h>

namespace Engine {

    namespace Graphics {

        struct Texture2DSettings {
            bool GenerateMipmaps = true;
            GLenum TextureMagnifyingFiltering = GL_LINEAR;
            GLenum TextureMinifyingFiltering = GL_NEAREST;
            GLenum TextureWrapping = GL_REPEAT;
            bool FlipTextureWhenLoadingFromFile = false;
        };

        class Texture2D {

        public:
            /**
             * Loads a texture from disk.
             * @param filepath The filepath to the texture.
             * @param settings The texture settings.
             */
            Texture2D(std::string filepath, Texture2DSettings *settings);

            /**
             * Creates an empty texture with format GL_RGB and data type GL_UNSIGNED_BYTE.
             * @param width Width of the texture.
             * @param height Height of the texture.
             * @param settings Texture settings.
             */
            Texture2D(uint32_t width, uint32_t height, Texture2DSettings *settings);

            /**
             * Creates an empty texture with the specified settings.
             * @param width Width of the texture.
             * @param height Height of the texture.
             * @param settings Texture settings.
             * @param pixelFormat PixelFormat e.g. GL_RGB or GL_RED
             * @param pixelDataType PixelDataType e.g. GL_UNSIGNED_BYTE or GL_FLOAT
             */
            Texture2D(uint32_t width, uint32_t height, Texture2DSettings *settings,
                      GLenum pixelFormat, GLenum pixelDataType);

            /**
             * Destroys this texture object and its corresponding data on the graphics card.
             */
            virtual ~Texture2D();

            /**
             * Binds the texture to a texture unit
             * @param unit The texture unit the texture should be bound to.
             * @note The texture unit should be between GL_TEXTURE0-GL_TEXTURE_MAX
             */
            void Bind(uint32_t unit);

            /**
             * Unbinds any texture.
             */
            void Unbind();

            unsigned int GetID();

            void SetData(std::vector<uint8_t>& data);



        private:
            uint32_t ID;
            int32_t channels;
            GLenum pixelFormat;
            GLenum pixelDataType;
            Texture2DSettings *settings = nullptr;
        public:
            uint32_t getWidth() const;

            uint32_t getHeight() const;

        private:
            uint32_t width;
            uint32_t height;

        private:

            void GenerateTexture();

            void DeleteTexture();

            void LoadFromFile(std::string filepath, Texture2DSettings *settings);

            void GenerateEmptyTexture(uint32_t width, uint32_t height, Texture2DSettings *settings);

            void GenerateEmptyTexture(uint32_t width, uint32_t height, Texture2DSettings *settings,
                                      GLenum pixelFormat, GLenum pixelDataType);

        };
    }
}


#endif //FLUIDSOLVER_TEXTURE2D_HPP
