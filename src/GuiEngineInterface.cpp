//
// Created by corbi on 22.04.2019.
//

#include <algorithm>
#include "GuiEngineInterface.hpp"


void GuiEngineInterface::DrawRectangle(float positionX, float positionY, float width, float height,
                                                   cppgui::Color color,
                                                   cppgui::Vector4 cliparea) {
    rectangleRenderer->RenderRectangle(glm::vec2(positionX, positionY), glm::vec2(width, height),
                                       glm::vec4(color.r, color.g, color.b, color.a),
                                       glm::vec4(cliparea.x, cliparea.y, cliparea.z, cliparea.w));
}

void
GuiEngineInterface::DrawString(std::string text, float positionX, float positionY, float size,
                                           cppgui::Color color,
                                           cppgui::Vector4 cliparea) {
    size *= 0.5f;
    textRenderer->Render(text, positionX, positionY, size, glm::vec4(color.r, color.g, color.b, color.a),
                         glm::vec4(cliparea.x, cliparea.y, cliparea.z, cliparea.w),
                         glm::vec4(cliparea.x, cliparea.y, cliparea.z, cliparea.w));
}

void
GuiEngineInterface::DrawTexture(cppgui::GuiInterfaceTexture *texture, float positionX, float positionY,
                                            float width,
                                            float height, cppgui::Vector4 cliparea) {
    // TODO: implement
}

cppgui::Vector2 GuiEngineInterface::GetTextDimension(std::string text, float size) {
    size *= 0.5f;
    glm::vec2 val = textRenderer->GetTextDimensions(text, size);
    return cppgui::Vector2(val.x, val.y);
}

float GuiEngineInterface::GetRenderTargetWidth() {
    return window->GetWidth();
}

float GuiEngineInterface::GetRenderTargetHeight() {
    return window->GetHeight();
}

GuiEngineInterface::GuiEngineInterface(Engine::Window *window, Engine::Text::Font *font) {
    this->window = window;
    this->font = font;

    textRenderer = new Engine::Text::TextRenderer(font);
    textRenderer->CreateProjectionMatrixForScreen(window->GetWidth(), window->GetHeight());


    rectangleRenderer = new Engine::RectangleRenderer();
    rectangleRenderer->CreateProjectionMatrixForScreen(window->GetWidth(), window->GetHeight());

    window->OnWindowSizeChanged.Subscribe([=](int width, int height) {
        textRenderer->CreateProjectionMatrixForScreen(width, height);
        rectangleRenderer->CreateProjectionMatrixForScreen(width, height);
    });

}
