//
// Created by corbi on 03.01.2020.
//

#include "MainUi.hpp"

#include <uiVersion/FluidSolverWindow.hpp>
#include <imguiHelper.hpp>
#include <core/timestep/ConstantTimestep.hpp>
#include <core/timestep/DynamicCFLTimestep.hpp>

FluidUI::MainUi::MainUi(FluidSolverWindow *window) : window(window) {}

void FluidUI::MainUi::Run() {
    ImGui::ShowDemoWindow();

    SimulationControl();
    Scenario();
    Simulation();

}

void FluidUI::MainUi::SimulationControl() {

    ImGui::Begin("Controls");


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

void FluidUI::MainUi::Simulation() {
    auto constant = dynamic_cast<FluidSolver::ConstantTimestep *>(window->GetTimestep());
    auto dynamic = dynamic_cast<FluidSolver::DynamicCFLTimestep *>(window->GetTimestep());

    ImGui::Begin("Simulation");

    if (!Simulation_Init) {
        Simulation_RestDensity = window->GetRestDensity();
        if (constant != nullptr) {
            Simulation_SelectedTimestep = 0;
        } else {
            Simulation_SelectedTimestep = 1;
        }
        Simulation_Init = true;
    }
    if (window->GetRestDensity() != Simulation_RestDensity) {
        window->SetRestDensity(Simulation_RestDensity);
    }


    ImGui::InputFloat("Rest Density", &Simulation_RestDensity);

    ImGui::Separator();

    static const char *selection[]{"Constant Timestep", "Dynamic CFL Timestep"};

    if (Simulation_SelectedTimestep == 0 && constant == nullptr) {
        // choose constant timestep
        auto tmp = new FluidSolver::ConstantTimestep(0.001f);
        delete window->GetTimestep();
        window->SetTimestep(tmp);
        constant = tmp;
        dynamic = nullptr;
    } else if (Simulation_SelectedTimestep == 1 && dynamic == nullptr) {
        // choose dynamic timestep
        auto tmp = new FluidSolver::DynamicCFLTimestep();
        delete window->GetTimestep();
        window->SetTimestep(tmp);
        dynamic = tmp;
        constant = nullptr;
    }

    ImGui::ListBox("Timestep", &Simulation_SelectedTimestep, selection, 2);

    ImGui::Text("Current Timestep: %f", window->GetTimestep()->getCurrentTimestep());

    ImGui::Text("Settings");
    if (constant != nullptr) {
        // constant timestep settings
        ImGui::InputFloat("Timestep", &constant->Timestep);
    } else if (dynamic != nullptr) {
        // dynamic timestep settings
        ImGui::InputFloat("Min. Timestep", &dynamic->MinimumTimestep, 0, 0, "%.6f");
        ImGui::InputFloat("Max. Timestep", &dynamic->MaximumTimestep, 0, 0, "%.6f");
        ImGui::InputFloat("CFL Number", &dynamic->CFLNumber, 0, 0, "%.6f");

    }


    ImGui::End();
}
