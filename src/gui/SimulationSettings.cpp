//
// Created by corbi on 09.07.2019.
//

#include "SimulationSettings.hpp"
#include <cppgui/src/AlignBox.hpp>
#include <cppgui/src/AlignSpread.hpp>
#include <cppgui/src/Spread.hpp>
#include <cppgui/src/Button.hpp>
#include <cppgui/src/ScrollBox.hpp>
#include <cppgui/src/Expander.hpp>
#include <cppgui/src/DirectionalSpread.hpp>
#include <cppgui/src/ComboBox.hpp>
#include <cppgui/src/TextInput.hpp>
#include <cppgui/src/ColorPickerExtended.hpp>
#include <cppgui/src/ExtendedSlider.hpp>


FluidSolver::Gui::SimulationSettings::SimulationSettings(ParticleRenderer *particleRenderer,
                                                         FluidSolverWindow *window) {
    this->particleRenderer = particleRenderer;
    this->window = window;
    setup();
}

void FluidSolver::Gui::SimulationSettings::setup() {
    auto al = new cppgui::AlignBox(cppgui::AlignmentTop, 50.0f);
    addChild(al);
    auto fill = new cppgui::AlignSpread();
    addChild(fill);

    auto viewButton = new cppgui::Button("Menue");
    al->addChild(new cppgui::Spread(viewButton));

    viewButton->OnClickEvent.Subscribe([=](float x, float y) {
        fill->Visible = !fill->Visible;
    });
    fill->Visible = false;

    auto scroll = new cppgui::ScrollBox();
    fill->addChild(new cppgui::Spread(scroll));

    auto stack = new cppgui::Stack();
    scroll->addChild(new cppgui::DirectionalSpread(stack));

    auto paramExpander = new cppgui::Expander("Parameters");
    stack->addChild(new cppgui::DirectionalSpread(paramExpander));
    auto paramExpanderStack = new cppgui::Stack();
    paramExpander->addChild(new cppgui::DirectionalSpread(paramExpanderStack));
    setupParameterExpanderStack(paramExpanderStack);

    auto colorExpander = new cppgui::Expander("Colors");
    stack->addChild(new cppgui::DirectionalSpread(colorExpander));
    auto colorExpanderStack = new cppgui::Stack();
    colorExpander->addChild(new cppgui::DirectionalSpread(colorExpanderStack));
    setupColorExpanderStack(colorExpanderStack);


    auto scenarionExpander = new cppgui::Expander("Scenarios");
    stack->addChild(new cppgui::DirectionalSpread(scenarionExpander));
    auto scenarioExpanderStack = new cppgui::Stack();
    scenarionExpander->addChild(new cppgui::DirectionalSpread(scenarioExpanderStack));
    setupScenarioExpanderStack(scenarioExpanderStack);

}

void FluidSolver::Gui::SimulationSettings::setupColorExpanderStack(cppgui::Stack *colorExpanderStack) {
    auto combobox = new cppgui::ComboBox();
    colorExpanderStack->addChild(new cppgui::DirectionalSpread(combobox));
    combobox->items = {"Velocity", "Acceleration", "Mass", "Pressure", "Density"};
    combobox->setSelectedIndex((int) this->particleRenderer->colorSelection - 1);
    combobox->OnSelectionChanged.Subscribe([=](int index, std::string newSelection) {
        this->particleRenderer->colorSelection = (ParticleRenderer::ColorSelection) (index + 1);
    });

    auto bottom = new cppgui::Label("Bottom Value:");
    auto colbottom = cppgui::Color(particleRenderer->bottomColor.x, particleRenderer->bottomColor.y,
                                   particleRenderer->bottomColor.z, particleRenderer->bottomColor.w);
    auto colorpickerBottom = new cppgui::ColorPickerExtended(180, 50, "Bottom Color:", colbottom);
    colorpickerBottom->OnColorChanged.Subscribe([=](cppgui::Color newColor) {
        this->particleRenderer->bottomColor = glm::vec4(newColor.r, newColor.g, newColor.b, newColor.a);
    });
    auto bottomInput = new cppgui::TextInput(200, 50, std::to_string(particleRenderer->bottomValue));
    bottomInput->OnTextChanged.Subscribe([=](std::string newText) {
        try {
            auto value = std::stof(newText);
            this->particleRenderer->bottomValue = value;
        } catch (std::exception &e) {

        }
    });
    colorExpanderStack->addChild(new cppgui::DirectionalSpread(colorpickerBottom));
    colorExpanderStack->addChild(new cppgui::DirectionalSpread(bottom));

    colorExpanderStack->addChild(new cppgui::DirectionalSpread(bottomInput));

    auto top = new cppgui::Label("Top:");
    auto coltop = cppgui::Color(particleRenderer->topColor.x, particleRenderer->topColor.y,
                                particleRenderer->topColor.z, particleRenderer->topColor.w);
    auto colorpickerTop = new cppgui::ColorPickerExtended(180, 50, "Top Color:", coltop);
    colorpickerTop->OnColorChanged.Subscribe([=](cppgui::Color newColor) {
        this->particleRenderer->topColor = glm::vec4(newColor.r, newColor.g, newColor.b, newColor.a);
    });
    auto topInput = new cppgui::TextInput(200, 50, std::to_string(particleRenderer->topValue));
    topInput->OnTextChanged.Subscribe([=](std::string newText) {
        try {
            auto value = std::stof(newText);
            this->particleRenderer->topValue = value;
        } catch (std::exception &e) {

        }
    });

    colorExpanderStack->addChild(new cppgui::DirectionalSpread(colorpickerTop));
    colorExpanderStack->addChild(new cppgui::DirectionalSpread(top));
    colorExpanderStack->addChild(new cppgui::DirectionalSpread(topInput));
}

void FluidSolver::Gui::SimulationSettings::setupParameterExpanderStack(cppgui::Stack *parameterExpanderStack) {
    auto k = new cppgui::Label("k:");
    auto kInp = new cppgui::TextInput(200, 50, std::to_string(window->sphFluidSolver->StiffnessK));
    kInp->OnTextChanged.Subscribe([=](std::string newText) {
        try {
            auto value = std::stof(newText);
            window->sphFluidSolver->StiffnessK = value;
        } catch (std::exception &e) {

        }
    });
    parameterExpanderStack->addChild(new cppgui::DirectionalSpread(k));
    parameterExpanderStack->addChild(new cppgui::DirectionalSpread(kInp));

    auto v = new cppgui::Label("v:");
    auto vInp = new cppgui::TextInput(200, 50, std::to_string(window->sphFluidSolver->Viscosity));
    vInp->OnTextChanged.Subscribe([=](std::string newText) {
        try {
            auto value = std::stof(newText);
            window->sphFluidSolver->Viscosity = value;
        } catch (std::exception &e) {

        }
    });
    parameterExpanderStack->addChild(new cppgui::DirectionalSpread(v));
    parameterExpanderStack->addChild(new cppgui::DirectionalSpread(vInp));

    auto extSlider = new cppgui::ExtendedSlider("RealTimeSpeed", this->window->RealTimeSpeed, 2.0f, 0.0f);
    extSlider->setWidth(250.0f);
    parameterExpanderStack->addChild(new cppgui::DirectionalSpread(extSlider));
    extSlider->OnValueChanged.Subscribe([=](float newValue) {
        this->window->RealTimeSpeed = newValue;
    });

}

void FluidSolver::Gui::SimulationSettings::setupScenarioExpanderStack(cppgui::Stack *scenarioExpanderStack) {
    auto resetBtn = new cppgui::Button("Reset", 100, 50);
    scenarioExpanderStack->addChild(new cppgui::DirectionalSpread(resetBtn));
    resetBtn->OnClickEvent.Subscribe([=](float x, float y) {
        window->resetData();
    });
}
