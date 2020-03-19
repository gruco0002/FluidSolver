#include "ContinousVisualizerOpenGL.hpp"

void ContinousVisualizerOpenGL::AfterRender(std::vector<Color> &data) {
    texture->SetData(data.data(), data.size() * 3);
}


ContinousVisualizerOpenGL::ContinousVisualizerOpenGL() : ContinousVisualizer(1920, 1080) {

    RecreateTexture();

}

ContinousVisualizerOpenGL::~ContinousVisualizerOpenGL() {
    delete texture;
}

void ContinousVisualizerOpenGL::setRenderTargetSize(size_t width, size_t height) {
    ContinousVisualizer::setRenderTargetSize(width, height);
    RecreateTexture();
}

void ContinousVisualizerOpenGL::RecreateTexture() {
    delete texture;

    auto settings = new Engine::Graphics::Texture2DSettings();
    settings->FlipTextureWhenLoadingFromFile = false;
    settings->GenerateMipmaps = false;

    texture = new Engine::Graphics::Texture2D(getWidth(), getHeight(), settings, GL_RGB,
                                              Engine::ComponentTypeUnsignedByte);

}

Engine::Graphics::Texture2D *ContinousVisualizerOpenGL::GetTexture() {
    return texture;
}

