//
// Created by corbi on 19.11.2019.
//

#ifndef CPPGUI_TEXTURE2DARRAY_HPP
#define CPPGUI_TEXTURE2DARRAY_HPP

#include <cstdint>
#include "Texture2D.hpp"

namespace Engine {

    namespace Graphics {

        class Texture2DArray {

        public:
            /**
             * Creates an empty texture with the specified settings.
             * @param width Width of the texture.
             * @param height Height of the texture.
             * @param settings Texture settings.
             * @param pixelFormat PixelFormat e.g. GL_RGB or GL_RED
             * @param pixelDataType PixelDataType e.g. GL_UNSIGNED_BYTE or GL_FLOAT
             * @param depth The depth of the texture array
             */
            Texture2DArray(uint32_t width, uint32_t height, uint32_t depth, Texture2DSettings *settings,
                           GLenum pixelFormat,GLenum sizedPixelFormat, ComponentType pixelDataType);

            /**
            * Sets the data of the texture object.
            * @param data A vector holding the new data.
            * @param depth The depth where the data should be set.
            * @note The data has to have the size of  width * height * channels.
            */
            void SetData(std::vector<uint8_t> &data, int32_t depth);

            /**
            * Sets the data of the texture object.
            * @param data A vector holding the new data.
            * @param depth The depth where the data should be set.
            * @param count The number of layer where the data should be written to.
            * @note The data has to have the size of count * width * height * channels.
            */
            void SetData(std::vector<uint8_t> &data, int32_t depth, int32_t count);

            /**
             * Sets the data of the texture object.
             * @param data A vector holding the new data.
             * @param x Specifies a pixel offset in x direction.
             * @param y Specifies a pixel offset in x direction.
             * @param z Specifies a pixel offset in x direction.
             * @param width The width of the texture region to be set.
             * @param height The height of the texture region to be set.
             * @param depth The depth of the texture region to be set.
             */
            void SetData(std::vector<uint8_t> &data, int32_t x, int32_t y, int32_t z,
                         int32_t width, int32_t height, int32_t depth);


            /**
             * Destroys this texture object and its corresponding data on the graphics card.
             */
            virtual ~Texture2DArray();

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


        private:
            uint32_t ID;
            uint32_t depth;
            int32_t channels;
            GLenum pixelFormat;
            GLenum sizedPixelFormat;
            ComponentType pixelDataType;
            Texture2DSettings *settings = nullptr;

        public:
            uint32_t getWidth() const;

            uint32_t getHeight() const;

        private:
            uint32_t width;
            uint32_t height;

            void GenerateTexture();

            void DeleteTexture();

        };
    }
}


#endif //CPPGUI_TEXTURE2DARRAY_HPP
