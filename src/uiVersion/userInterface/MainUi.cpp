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
    Scenario();

}

void FluidUI::MainUi::SimulationControl() {

    ImGui::Begin("Simulation");


    if (ImGui::Button(window->Pause ? "Play" : "Pause")) {
        window->Pause = !window->Pause;
    }

    if (ImGui::Button("Reset")) {
        window->resetData();
    }

    ImGui::End();
}

void FluidUI::MainUi::Scenario() {
    ImGui::Begin("Scenario");

    ImGui::Text("Current Scenario:");
    ImGui::Text("Name: %s", window->GetScenario()->GetName().c_str());
    ImGui::Text("Particle Size: %f", window->GetScenario()->GetParticleSize());
    ImGui::Separator();
    ImGui::Text("Load Scenario");

    std::vector<const char *> scenarios;
    Scenario_Names.clear();
    int i = 0;
    for (auto scn : window->Scenarios) {

        Scenario_Names.push_back(scn->GetName());
        scenarios.push_back(Scenario_Names[Scenario_Names.size() - 1].c_str());
        if (Scenario_selectedScenario == i && Scenario_init) {
            // choose new scenario
            if (scn->GetName() != window->GetScenario()->GetName()) {
                // set new scenario
                window->SetScenario(scn);
            }
        }

        if (scn->GetName() == window->GetScenario()->GetName()) {
            Scenario_init = true;
        }
        if (!Scenario_init) {
            Scenario_selectedScenario++;
        }

        i++;
    }

    ImGui::ListBox("Scenarios", &Scenario_selectedScenario, scenarios.data(), scenarios.size());

    ImGui::End();
}
