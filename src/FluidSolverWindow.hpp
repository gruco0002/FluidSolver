//
// Created by corbi on 17.04.2019.
//

#ifndef FLUIDSOLVER_FLUIDSOLVERWINDOW_HPP
#define FLUIDSOLVER_FLUIDSOLVERWINDOW_HPP


#include <engine/Window.hpp>
#include <engine/text/Font.hpp>
#include <engine/text/TextRenderer.hpp>

class FluidSolverWindow : public Engine::Window {

public:
    FluidSolverWindow(const std::string &title, int width = 800, int height = 600);

    static bool even(int input);

private:
    Engine::Text::Font* font;

    void loadFont();

    Engine::Text::TextRenderer* textRenderer;

protected:
    void render() override;

    void load() override;

};


#endif //FLUIDSOLVER_FLUIDSOLVERWINDOW_HPP
