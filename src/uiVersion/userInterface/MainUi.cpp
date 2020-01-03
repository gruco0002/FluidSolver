//
// Created by corbi on 03.01.2020.
//

#include "MainUi.hpp"

#include <uiVersion/FluidSolverWindow.hpp>
#include <imguiHelper.hpp>

FluidUI::MainUi::MainUi(FluidSolverWindow *window) : window(window) {}

void FluidUI::MainUi::Run() {
    ImGui::ShowDemoWindow();

    SimulationControl();

}

void FluidUI::MainUi::SimulationControl() {

    ImGui::Begin("Simulation");


    if (ImGui::Button(window->Pause ? "Play" : "Pause")) {
        window->Pause = !window->Pause;
    }


    ImGui::End();
}
