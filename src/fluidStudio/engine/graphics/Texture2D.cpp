#include "Texture2D.hpp"

#include "Framebuffer.hpp"
#include "stb_image.h"
#include "stb_image_write.h"

#include <fstream>
#include <iostream>


void Engine::Graphics::Texture2D::LoadFromFile(std::string filepath, Engine::Graphics::Texture2DSettings* settings) {
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
    pixelDataType = ComponentTypeUnsignedByte;

    int width, height;
    unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);
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
        glTexImage2D(GL_TEXTURE_2D, 0, pixelFormat, width, height, 0, pixelFormat, pixelDataType, data);
        if (settings == nullptr || settings->GenerateMipmaps)
            glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        throw std::invalid_argument(("Failed to load texture: " + filepath).c_str());
    }
    stbi_image_free(data);
    stbi_set_flip_vertically_on_load(false);
}

void Engine::Graphics::Texture2D::GenerateEmptyTexture(uint32_t width, uint32_t height,
        Engine::Graphics::Texture2DSettings* settings) {
    GenerateEmptyTexture(width, height, settings, GL_RGB, GL_RGB, ComponentTypeUnsignedByte);
}

void Engine::Graphics::Texture2D::GenerateEmptyTexture(uint32_t width, uint32_t height,
        Engine::Graphics::Texture2DSettings* settings,
        GLenum pixelFormat, GLenum internalPixelFormat,
        ComponentType pixelDataType) {
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
            throw std::invalid_argument("Failed to generate texture: Pixel format not supported");
    }

    glBindTexture(GL_TEXTURE_2D, ID);
    if (settings != nullptr) {
        // set the texture wrapping/filtering options (on the currently bound texture object)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, settings->TextureWrapping);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, settings->TextureWrapping);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, settings->TextureMinifyingFiltering);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, settings->TextureMagnifyingFiltering);
    }
    glTexImage2D(GL_TEXTURE_2D, 0, internalPixelFormat, width, height, 0, pixelFormat, pixelDataType, nullptr);
}

Engine::Graphics::Texture2D::~Texture2D() {
    DeleteTexture();
    delete settings;
}

Engine::Graphics::Texture2D::Texture2D(std::string filepath, Engine::Graphics::Texture2DSettings* settings) {
    GenerateTexture();
    LoadFromFile(filepath, settings);
}

Engine::Graphics::Texture2D::Texture2D(uint32_t width, uint32_t height, Engine::Graphics::Texture2DSettings* settings) {
    GenerateTexture();
    GenerateEmptyTexture(width, height, settings);
}

Engine::Graphics::Texture2D::Texture2D(uint32_t width, uint32_t height, Engine::Graphics::Texture2DSettings* settings,
        GLenum pixelFormat, ComponentType pixelDataType) {
    GenerateTexture();
    GenerateEmptyTexture(width, height, settings, pixelFormat, pixelFormat, pixelDataType);
}

Engine::Graphics::Texture2D::Texture2D(uint32_t width, uint32_t height, Engine::Graphics::Texture2DSettings* settings,
        GLenum pixelFormat, GLenum internalPixelFormat, ComponentType pixelDataType) {
    GenerateTexture();
    GenerateEmptyTexture(width, height, settings, pixelFormat, internalPixelFormat, pixelDataType);
}

void Engine::Graphics::Texture2D::GenerateTexture() {
    glGenTextures(1, &ID);
}

void Engine::Graphics::Texture2D::DeleteTexture() {
    Unbind();
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

void Engine::Graphics::Texture2D::SetData(const std::vector<uint8_t>& data) {
    SetData(data.data(), data.size());
}

uint32_t Engine::Graphics::Texture2D::getWidth() const {
    return width;
}

uint32_t Engine::Graphics::Texture2D::getHeight() const {
    return height;
}

void Engine::Graphics::Texture2D::SaveAsPNG(std::string filepath) {
    auto img = GetData();
    // stbi_flip_vertically_on_write(true);
    stbi_write_png(filepath.c_str(), width, height, channels, img.data(), channels * width);
    // stbi_flip_vertically_on_write(false);
}

std::vector<uint8_t> Engine::Graphics::Texture2D::GetData() {
    Framebuffer framebuffer(width, height);
    framebuffer.AddAttachment(GL_COLOR_ATTACHMENT0, this);

    std::vector<uint8_t> ret(width * height * channels * SizeOfComponentType(pixelDataType));
    framebuffer.Bind();
    glReadPixels(0, 0, width, height, pixelFormat, pixelDataType, ret.data());
    framebuffer.Unbind();

    return ret;
}


void Engine::Graphics::Texture2D::SaveAsBinaryPPM(std::string filepath) {
    auto img = GetData();
    FlipYDataOfArray(img);
    std::ofstream file;
    file.open(filepath, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!file.is_open())
        return;

    file << "P6\n" + std::to_string(width) + " " + std::to_string(height) + "\t255" + "\n";
    file.write((const char*)img.data(), img.size());
    file.close();
}

void Engine::Graphics::Texture2D::FlipYDataOfArray(std::vector<uint8_t>& data) {
    for (size_t x = 0; x < width; x++) {
        for (size_t y = 0; y < height / 2; y++) {
            for (size_t c = 0; c < channels; c++) {
                size_t index1 = (y * width + x) * channels + c;
                size_t index2 = ((height - 1 - y) * width + x) * channels + c;
                uint8_t val1 = data[index1];
                uint8_t val2 = data[index2];
                data[index1] = val2;
                data[index2] = val1;
            }
        }
    }
}

void Engine::Graphics::Texture2D::SetData(const void* data, size_t length) {
    glBindTexture(GL_TEXTURE_2D, ID);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, pixelFormat, pixelDataType, data);

    if (settings != nullptr && settings->GenerateMipmaps)
        glGenerateMipmap(GL_TEXTURE_2D);
}
