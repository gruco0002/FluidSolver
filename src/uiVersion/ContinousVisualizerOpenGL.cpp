//
// Created by Corbinian Gruber on 19.12.19.
//

#include "ContinousVisualizerOpenGL.hpp"

void ContinousVisualizerOpenGL::AfterRender(std::vector<Color> &data) {
    texture->SetData(data.data(), data.size() * 3);
    rectangleRenderer->RenderTexture(glm::vec2(0.0f), glm::vec2((float) getWidth(), (float) getHeight()), texture);
}

void ContinousVisualizerOpenGL::SetScenarioSize(FluidSolver::Scenario *scenario) {
    int particlesX = scenario->GetParticleCountX();
    int particlesY = scenario->GetParticleCountY();

    float width = particlesX;
    float height = particlesY;

    Viewport port;
    port.Left = -width / 2.0f;
    port.Top = -height / 2.0f;
    port.Width = width;
    port.Height = height;

    viewport = FitViewportToAspectRation(port);
}

ContinousVisualizerOpenGL::ContinousVisualizerOpenGL() : ContinousVisualizer(1920, 1080) {

    auto settings = new Engine::Graphics::Texture2DSettings();
    settings->FlipTextureWhenLoadingFromFile = false;
    settings->GenerateMipmaps = false;

    texture = new Engine::Graphics::Texture2D(getWidth(), getHeight(), settings, GL_RGB,
                                              Engine::ComponentTypeUnsignedByte);

    rectangleRenderer = new Engine::RectangleRenderer();
    rectangleRenderer->CreateProjectionMatrixForScreen(getWidth(), getHeight());

}

ContinousVisualizerOpenGL::~ContinousVisualizerOpenGL() {
    delete texture;
}

