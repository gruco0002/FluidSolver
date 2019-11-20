//
// Created by corbi on 22.04.2019.
//

#ifndef CPPGUI_DEMO_GUIENGINEINTERFACE_HPP
#define CPPGUI_DEMO_GUIENGINEINTERFACE_HPP


#include <engine/Window.hpp>
#include <engine/RectangleRenderer.hpp>
#include <engine/text/TextRenderer.hpp>
#include <dependencies/cppgui/library/src/interface/GuiInterface.hpp>


    class GuiEngineInterface : public cppgui::GuiInterface {
    public:

        GuiEngineInterface(Engine::Window *window, Engine::Text::Font *font);

        void DrawRectangle(float positionX, float positionY, float width, float height, cppgui::Color color,
                           cppgui::Vector4 cliparea) override;

        void DrawString(std::string text, float positionX, float positionY, float size, cppgui::Color color,
                        cppgui::Vector4 cliparea) override;

        void
        DrawTexture(cppgui::GuiInterfaceTexture *texture, float positionX, float positionY, float width, float height,
                    cppgui::Vector4 cliparea) override;

        cppgui::Vector2 GetTextDimension(std::string text, float size) override;

        float GetRenderTargetWidth() override;

        float GetRenderTargetHeight() override;

    private:
        Engine::Window *window;
        Engine::RectangleRenderer *rectangleRenderer;
        Engine::Text::TextRenderer *textRenderer;
        Engine::Text::Font *font;
    };


#endif //CPPGUI_DEMO_GUIENGINEINTERFACE_HPP
