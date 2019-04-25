//
// Created by corbi on 24.04.2019.
//

#include "FluidSolverTopMenu.hpp"
#include <dependencies/cppgui/src/Button.hpp>
#include <dependencies/cppgui/src/TextInput.hpp>
#include <dependencies/cppgui/src/AlignBox.hpp>
#include <dependencies/cppgui/src/Stack.hpp>
#include <dependencies/cppgui/src/DirectionalSpread.hpp>
#include <dependencies/cppgui/src/ComboBox.hpp>
#include <dependencies/cppgui/src/StyledLabel.hpp>
#include <dependencies/cppgui/src/ColorPickerExtended.hpp>
#include <dependencies/cppgui/src/Center.hpp>
#include "FluidSolverWindow.hpp"

FluidSolverTopMenu::FluidSolverTopMenu(ParticleRenderer *particleRenderer, FluidSolverWindow *window) : cppgui::Panel(0,
                                                                                                                      0,
                                                                                                                      0,
                                                                                                                      0) {
    this->particleRenderer = particleRenderer;
    this->window = window;
    Setup();
}

void FluidSolverTopMenu::Setup() {
    auto stack = new cppgui::Stack(cppgui::StackHorizontal);
    stack->setMarginBetween(10.0f);

    backgroundColor = cppgui::Color(1.0f);


    addChild(new cppgui::DirectionalSpread(stack, cppgui::SpreadDirectionVertical));

    auto combobox = new cppgui::ComboBox(200, 0);
    stack->addChild(new cppgui::DirectionalSpread(combobox, cppgui::SpreadDirectionVertical));
    combobox->items = {"Velocity", "Acceleration", "Mass", "Pressure", "Density"};
    combobox->setSelectedIndex((int) this->particleRenderer->colorSelection - 1);
    combobox->OnSelectionChanged.Subscribe([=](int index, std::string newSelection) {
        this->particleRenderer->colorSelection = (ParticleRenderer::ColorSelection) (index + 1);
    });

    auto bottom = new cppgui::Label("Bottom:");
    auto colbottom = cppgui::Color(particleRenderer->bottomColor.x, particleRenderer->bottomColor.y,
                                   particleRenderer->bottomColor.z, particleRenderer->bottomColor.w);
    auto colorpickerBottom = new cppgui::ColorPickerExtended(180, 50, "Bottom Color:", colbottom);
    colorpickerBottom->OnColorChanged.Subscribe([=](cppgui::Color newColor) {
        this->particleRenderer->bottomColor = glm::vec4(newColor.r, newColor.g, newColor.b, newColor.a);
    });
    auto bottomInput = new cppgui::TextInput(200, 0, std::to_string(particleRenderer->bottomValue));
    bottomInput->OnTextChanged.Subscribe([=](std::string newText) {
        try {
            auto value = std::stof(newText);
            this->particleRenderer->bottomValue = value;
        } catch (std::exception &e) {

        }
    });

    stack->addChild(new cppgui::Center(colorpickerBottom, cppgui::Center::CenterDirectionVertical));
    stack->addChild(new cppgui::Center(bottom, cppgui::Center::CenterDirectionVertical));

    stack->addChild(new cppgui::DirectionalSpread(bottomInput, cppgui::SpreadDirectionVertical));


    auto top = new cppgui::Label("Top:");
    auto coltop = cppgui::Color(particleRenderer->topColor.x, particleRenderer->topColor.y,
                                particleRenderer->topColor.z, particleRenderer->topColor.w);
    auto colorpickerTop = new cppgui::ColorPickerExtended(180, 50, "Top Color:", coltop);
    colorpickerTop->OnColorChanged.Subscribe([=](cppgui::Color newColor) {
        this->particleRenderer->topColor = glm::vec4(newColor.r, newColor.g, newColor.b, newColor.a);
    });
    auto topInput = new cppgui::TextInput(200, 0, std::to_string(particleRenderer->topValue));
    topInput->OnTextChanged.Subscribe([=](std::string newText) {
        try {
            auto value = std::stof(newText);
            this->particleRenderer->topValue = value;
        } catch (std::exception &e) {

        }
    });

    stack->addChild(new cppgui::Center(colorpickerTop, cppgui::Center::CenterDirectionVertical));
    stack->addChild(new cppgui::Center(top, cppgui::Center::CenterDirectionVertical));
    stack->addChild(new cppgui::DirectionalSpread(topInput, cppgui::SpreadDirectionVertical));

    auto k = new cppgui::Label("k:");
    auto kInp = new cppgui::TextInput(200, 0, std::to_string(window->sphFluidSolver->StiffnessK));
    kInp->OnTextChanged.Subscribe([=](std::string newText) {
        try {
            auto value = std::stof(newText);
            window->sphFluidSolver->StiffnessK = value;
        } catch (std::exception &e) {

        }
    });
    stack->addChild(new cppgui::Center(k, cppgui::Center::CenterDirectionVertical));
    stack->addChild(new cppgui::DirectionalSpread(kInp, cppgui::SpreadDirectionVertical));

    auto v = new cppgui::Label("v:");
    auto vInp = new cppgui::TextInput(200, 0, std::to_string(window->sphFluidSolver->Viscosity));
    vInp->OnTextChanged.Subscribe([=](std::string newText) {
        try {
            auto value = std::stof(newText);
            window->sphFluidSolver->Viscosity = value;
        } catch (std::exception &e) {

        }
    });
    stack->addChild(new cppgui::Center(v, cppgui::Center::CenterDirectionVertical));
    stack->addChild(new cppgui::DirectionalSpread(vInp, cppgui::SpreadDirectionVertical));


    auto resetBtn = new cppgui::Button("Reset", 100, 50);
    stack->addChild(new cppgui::DirectionalSpread(resetBtn, cppgui::SpreadDirectionVertical));
    resetBtn->OnClickEvent.Subscribe([=](float x, float y) {
        window->resetData();
    });
}
