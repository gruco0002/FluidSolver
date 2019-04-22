//
// Created by corbi on 22.04.2019.
//

#include <algorithm>
#include <iostream>
#include "GuiEngineInterface.hpp"

void GuiEngineInterface::DrawIcon(cppgui::FontSymbol symbol, cppgui::Color color, float positionX, float positionY,
                                  float width, float height, cppgui::Vector4 cliparea) {
    // TODO: implement correctly
    rectangleRenderer->RenderRectangle(glm::vec2(positionX, positionY), glm::vec2(width, height),
                                       glm::vec4(color.r, color.g, color.b, color.a),
                                       glm::vec4(cliparea.x, cliparea.y, cliparea.z, cliparea.w));
}

void GuiEngineInterface::DrawRectangle(float positionX, float positionY, float width, float height, cppgui::Color color,
                                       cppgui::Vector4 cliparea) {
    rectangleRenderer->RenderRectangle(glm::vec2(positionX, positionY), glm::vec2(width, height),
                                       glm::vec4(color.r, color.g, color.b, color.a),
                                       glm::vec4(cliparea.x, cliparea.y, cliparea.z, cliparea.w));
    std::cout << "testing" << std::endl;
}

void
GuiEngineInterface::DrawString(std::string text, float positionX, float positionY, float size, cppgui::Color color,
                             cppgui::Vector4 cliparea) {
    textRenderer->Render(text, size, glm::vec2(positionX, positionY), glm::vec4(color.r, color.g, color.b, color.a),
                         glm::vec4(cliparea.x, cliparea.y, cliparea.z, cliparea.w));
}

void
GuiEngineInterface::DrawTexture(cppgui::GuiInterfaceTexture *texture, float positionX, float positionY, float width,
                                float height, cppgui::Vector4 cliparea) {
    // TODO: implement
}

cppgui::Vector2 GuiEngineInterface::GetTextDimension(std::string text, float size) {
    if (text == "")return cppgui::Vector2(0.0f);
    auto vert = font->getTextVertices(text, size);
    float minX, minY, maxX, maxY;
    minX = vert[0].position.x;
    maxX = vert[0].position.x;
    minY = vert[0].position.y;
    maxY = vert[0].position.y;
    for (auto v : vert) {
        minX = std::min(minX, v.position.x);
        minY = std::min(minY, v.position.y);
        maxX = std::max(maxX, v.position.x);
        maxY = std::max(maxY, v.position.y);
    }

    return cppgui::Vector2(maxX - minX, maxY - minY);
}

float GuiEngineInterface::GetRenderTargetWidth() {
    return window->GetFramebufferWidth();
}

float GuiEngineInterface::GetRenderTargetHeight() {
    return window->GetFramebufferHeight();
}

GuiEngineInterface::GuiEngineInterface(Engine::Window *window, Engine::Text::Font *font) {
    this->window = window;
    this->font = font;

    textRenderer = new Engine::Text::TextRenderer(font);
    textRenderer->CreateProjectionMatrixForScreen(window->GetFramebufferWidth(), window->GetFramebufferHeight());

    rectangleRenderer = new Engine::RectangleRenderer();
    rectangleRenderer->CreateProjectionMatrixForScreen(window->GetFramebufferWidth(), window->GetFramebufferHeight());

    window->OnFramebufferSizeChanged.Subscribe([=](int width, int height) {
        textRenderer->CreateProjectionMatrixForScreen(width, height);
        rectangleRenderer->CreateProjectionMatrixForScreen(width, height);
    });

}
