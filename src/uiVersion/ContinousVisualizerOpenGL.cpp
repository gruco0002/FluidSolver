//
// Created by Corbinian Gruber on 19.12.19.
//

#include "ContinousVisualizerOpenGL.hpp"

void ContinousVisualizerOpenGL::AfterRender(std::vector<Color> &data) {
    texture->SetData(data.data(), data.size() * 3);
    rectangleRenderer->RenderTexture(glm::vec2(0.0f, (float) getHeight()),
                                     glm::vec2((float) getWidth(), -((float) getHeight())), texture);
}


ContinousVisualizerOpenGL::ContinousVisualizerOpenGL() : ContinousVisualizer(1920, 1080) {

    RecreateTexture();
    rectangleRenderer = new Engine::RectangleRenderer();
    rectangleRenderer->CreateProjectionMatrixForScreen(getWidth(), getHeight());

}

ContinousVisualizerOpenGL::~ContinousVisualizerOpenGL() {
    delete texture;
    delete rectangleRenderer;
}

void ContinousVisualizerOpenGL::setRenderTargetSize(size_t width, size_t height) {
    ContinousVisualizer::setRenderTargetSize(width, height);
    if (rectangleRenderer != nullptr)
        rectangleRenderer->CreateProjectionMatrixForScreen(getWidth(), getHeight());
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

