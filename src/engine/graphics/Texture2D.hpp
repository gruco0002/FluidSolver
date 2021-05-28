#ifndef ENGINE_TEXTURE2D_HPP
#define ENGINE_TEXTURE2D_HPP


#include <glad/glad.h>

#include "Enums.hpp"

#include <string>
#include <vector>

namespace Engine
{

    namespace Graphics
    {

        struct Texture2DSettings
        {
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

             * * @param settings The texture settings.
             */
            Texture2D(std::string filepath, Texture2DSettings* settings);

            /**
             * Creates an empty texture with format GL_RGB and data type GL_UNSIGNED_BYTE.
 * @param
             * width Width of the texture.
             * @param height Height of the texture.
             * @param
             * settings Texture settings.
             */
            Texture2D(uint32_t width, uint32_t height, Texture2DSettings* settings);

            /**
             * Creates an empty texture with the specified settings.
             * @param width Width
             * of the texture.
             * @param height Height of the texture.
             * @param settings
             * Texture settings.
             * @param pixelFormat PixelFormat e.g. GL_RGB or GL_RED
             *
             * @param pixelDataType PixelDataType e.g. GL_UNSIGNED_BYTE or GL_FLOAT
             */
            Texture2D(uint32_t width, uint32_t height, Texture2DSettings* settings, GLenum pixelFormat,
                      ComponentType pixelDataType);

            /**
             * Creates an empty texture with the specified settings.
             * @param width Width
             * of the texture.
             * @param height Height of the texture.
             * @param settings
             * Texture settings.
             * @param pixelFormat PixelFormat e.g. GL_RGB or GL_RED
             *
             * @param internalPixelFormat Sized Pixel Format: eg GL_R8, GL_R16
             * @param pixelDataType
             * PixelDataType e.g. GL_UNSIGNED_BYTE or GL_FLOAT
             */
            Texture2D(uint32_t width, uint32_t height, Texture2DSettings* settings, GLenum pixelFormat,
                      GLenum internalPixelFormat, ComponentType pixelDataType);

            /**
             * Destroys this texture object and its corresponding data on the graphics card.
 */
            virtual ~Texture2D();

            /**
             * Binds the texture to a texture unit
             * @param unit The texture unit the
             * texture should be bound to.
             * @note The texture unit should be between
             * GL_TEXTURE0-GL_TEXTURE_MAX
             */
            void Bind(uint32_t unit);

            /**
             * Unbinds any texture.
             */
            void Unbind();

            unsigned int GetID();

            void SetData(const std::vector<uint8_t>& data);

            void SetData(const void* data, size_t length);

            std::vector<uint8_t> GetData();

            void SaveAsPNG(std::string filepath);

            void SaveAsBinaryPPM(std::string filepath);

          private:
            uint32_t ID;
            int32_t channels;
            GLenum pixelFormat;
            ComponentType pixelDataType;
            Texture2DSettings* settings = nullptr;

          public:
            uint32_t getWidth() const;

            uint32_t getHeight() const;

          private:
            uint32_t width;
            uint32_t height;

          private:
            void GenerateTexture();

            void DeleteTexture();

            void LoadFromFile(std::string filepath, Texture2DSettings* settings);

            void GenerateEmptyTexture(uint32_t width, uint32_t height, Texture2DSettings* settings);

            void GenerateEmptyTexture(uint32_t width, uint32_t height, Texture2DSettings* settings, GLenum pixelFormat,
                                      GLenum internalPixelFormat, ComponentType pixelDataType);


            void FlipYDataOfArray(std::vector<uint8_t>& data);
        };
    } // namespace Graphics
} // namespace Engine


#endif // ENGINE_TEXTURE2D_HPP
