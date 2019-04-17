//
// Created by corbi on 17.04.2019.
//

#include <engine/EngineException.hpp>
#include "Texture2D.hpp"
#include "libraries/stb/stb_image.h"


void Engine::Graphics::Texture2D::LoadFromFile(std::string filepath, Engine::Graphics::Texture2DSettings *settings) {
    glBindTexture(GL_TEXTURE_2D, ID);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    if (settings != nullptr) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, settings->TextureWrapping);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, settings->TextureWrapping);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, settings->TextureMinifyingFiltering);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, settings->TextureMagnifyingFiltering);
        stbi_set_flip_vertically_on_load(settings->FlipTextureWhenLoadingFromFile);
    } else {
        stbi_set_flip_vertically_on_load(false);
    }
    this->settings = settings;
    pixelDataType = GL_UNSIGNED_BYTE;

    int width, height;
    unsigned char *data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);
    this->width = width;
    this->height = height;


    switch (channels) {
        case 1:
            pixelFormat = GL_RED;
            break;
        case 2:
            pixelFormat = GL_RG;
            break;
        case 3:
            pixelFormat = GL_RGB;
            break;
        case 4:
            pixelFormat = GL_RGBA;
            break;
    }

    if (data && pixelFormat != 0) {
        glTexImage2D(GL_TEXTURE_2D, 0, pixelFormat, width, height, 0, pixelFormat, pixelDataType,
                     data);
        if (settings == nullptr || settings->GenerateMipmaps)
            glGenerateMipmap(GL_TEXTURE_2D);

    } else {
        throw EngineException(("Failed to load texture: " + filepath).c_str());
    }
    stbi_image_free(data);
    stbi_set_flip_vertically_on_load(false);
}

void Engine::Graphics::Texture2D::GenerateEmptyTexture(uint32_t width, uint32_t height,
                                                       Engine::Graphics::Texture2DSettings *settings) {
    GenerateEmptyTexture(width, height, settings, GL_RGB, GL_UNSIGNED_BYTE);
}

void Engine::Graphics::Texture2D::GenerateEmptyTexture(uint32_t width, uint32_t height,
                                                       Engine::Graphics::Texture2DSettings *settings,
                                                       GLenum pixelFormat, GLenum pixelDataType) {
    this->pixelFormat = pixelFormat;
    this->pixelDataType = pixelDataType;
    this->settings = settings;
    this->width = width;
    this->height = height;

    switch (pixelFormat) {
        case GL_RED:
        case GL_GREEN:
        case GL_BLUE:
        case GL_ALPHA:
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

    glBindTexture(GL_TEXTURE_2D, ID);
    if (settings != nullptr) {
        // set the texture wrapping/filtering options (on the currently bound texture object)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, settings->TextureWrapping);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, settings->TextureWrapping);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, settings->TextureMinifyingFiltering);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, settings->TextureMagnifyingFiltering);
    }
    glTexImage2D(GL_TEXTURE_2D, 0, pixelFormat, width, height, 0, pixelFormat, pixelDataType, nullptr);
}

Engine::Graphics::Texture2D::~Texture2D() {
    DeleteTexture();
    delete settings;
}

Engine::Graphics::Texture2D::Texture2D(std::string filepath, Engine::Graphics::Texture2DSettings *settings) {
    GenerateTexture();
    LoadFromFile(filepath, settings);
}

Engine::Graphics::Texture2D::Texture2D(uint32_t width, uint32_t height,
                                       Engine::Graphics::Texture2DSettings *settings) {
    GenerateTexture();
    GenerateEmptyTexture(width, height, settings);
}

Engine::Graphics::Texture2D::Texture2D(uint32_t width, uint32_t height,
                                       Engine::Graphics::Texture2DSettings *settings, GLenum pixelFormat,
                                       GLenum pixelDataType) {
    GenerateTexture();
    GenerateEmptyTexture(width, height, settings, pixelFormat, pixelDataType);
}

void Engine::Graphics::Texture2D::GenerateTexture() {
    glGenTextures(1, &ID);
}

void Engine::Graphics::Texture2D::DeleteTexture() {
    glDeleteTextures(1, &ID);
}

void Engine::Graphics::Texture2D::Bind(uint32_t unit) {
    glActiveTexture(unit);
    glBindTexture(GL_TEXTURE_2D, ID);
}

void Engine::Graphics::Texture2D::Unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned int Engine::Graphics::Texture2D::GetID() {
    return ID;
}

void Engine::Graphics::Texture2D::SetData(std::vector<uint8_t> &data) {
    glBindTexture(GL_TEXTURE_2D, ID);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height,
                    pixelFormat, pixelDataType, data.data());

    if (settings->GenerateMipmaps)
        glGenerateMipmap(GL_TEXTURE_2D);
}

uint32_t Engine::Graphics::Texture2D::getWidth() const {
    return width;
}

uint32_t Engine::Graphics::Texture2D::getHeight() const {
    return height;
}
