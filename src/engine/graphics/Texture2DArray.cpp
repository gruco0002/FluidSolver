#include <engine/EngineException.hpp>
#include "Texture2DArray.hpp"

namespace Engine {

    namespace Graphics {


        Texture2DArray::Texture2DArray(uint32_t width, uint32_t height, uint32_t depth, Texture2DSettings *settings,
                                       GLenum pixelFormat, GLenum sizedPixelFormat, ComponentType pixelDataType) {
            this->width = width;
            this->height = height;
            this->depth = depth;

            this->settings = settings;
            this->pixelFormat = pixelFormat;
            this->pixelDataType = pixelDataType;
            this->sizedPixelFormat = sizedPixelFormat;

            GenerateTexture();
        }

        void Texture2DArray::SetData(std::vector<uint8_t> &data, int32_t depth) {
            SetData(data, depth, 1);
        }

        void Texture2DArray::SetData(std::vector<uint8_t> &data, int32_t depth, int32_t count) {
            SetData(data, 0, 0, depth, width, height, count);
        }

        void Texture2DArray::SetData(std::vector<uint8_t> &data, int32_t x, int32_t y, int32_t z, int32_t width,
                                     int32_t height, int32_t count) {
            glBindTexture(GL_TEXTURE_2D_ARRAY, ID);
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, x, y, z, width, height, count,
                            pixelFormat, pixelDataType, data.data());


            if (settings != nullptr && settings->GenerateMipmaps)
                glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
            Unbind();
        }

        Texture2DArray::~Texture2DArray() {
            DeleteTexture();
            delete settings;
        }

        void Texture2DArray::Bind(uint32_t unit) {
            glActiveTexture(unit);
            glBindTexture(GL_TEXTURE_2D_ARRAY, ID);
        }

        void Texture2DArray::Unbind() {
            glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        }

        uint32_t Texture2DArray::getWidth() const {
            return this->width;
        }

        uint32_t Texture2DArray::getHeight() const {
            return this->height;
        }

        void Texture2DArray::GenerateTexture() {
            glGenTextures(1, &ID);

            switch (pixelFormat) {
                case GL_RED:
                case GL_R8:
                case GL_GREEN:
                case GL_BLUE:
                case GL_ALPHA:
                case GL_DEPTH_COMPONENT:
                    channels = 1;
                    break;
                case GL_RG:
                    channels = 2;
                    break;
                case GL_RGB:
                case GL_BGR:
                    channels = 3;
                    break;
                case GL_RGBA:
                case GL_BGRA:
                    channels = 4;
                    break;
                default:
                    throw EngineException("Failed to generate texture: Pixel format not supported");
            }

            glBindTexture(GL_TEXTURE_2D_ARRAY, ID);

            if (settings != nullptr) {
                // set the texture wrapping/filtering options (on the currently bound texture object)
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, settings->TextureWrapping);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, settings->TextureWrapping);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, settings->TextureMinifyingFiltering);
                glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, settings->TextureMagnifyingFiltering);
            }

            glTexImage3D(GL_TEXTURE_2D_ARRAY,
                         0,                // level
                         sizedPixelFormat,         // Internal format
                         width, height, depth, // width,height,depth
                         0,
                         pixelFormat,          // format
                         pixelDataType, // type
                         nullptr);


            Unbind();
        }

        void Texture2DArray::DeleteTexture() {
            glDeleteTextures(1, &ID);
        }

        unsigned int Texture2DArray::GetID() {
            return ID;
        }
    }
}