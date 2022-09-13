#include "SimulationControlsWindow.hpp"

#include <ImguiHelper.hpp>

namespace FluidStudio {
    void SimulationControlsWindow::update() {
        ImGui::Begin("Simulation Controls");
        if (ui_data.window().simulation_should_run) {
            ImGui::TextColored(ImColor(0.1f, 0.8f, 0.1f), "Running");
        } else if (ui_data.window().are_calculations_running()) {
            ImGui::TextColored(ImColor(0.8f, 0.8f, 0.1f), "Paused, pending calculations");
        } else {
            ImGui::TextColored(ImColor(0.8f, 0.1f, 0.1f), "Paused");
        }

        if (ImGui::Button("Run")) {
            ui_data.window().simulation_should_run = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Pause")) {
            ui_data.window().simulation_should_run = false;
        }

        ImGui::Checkbox("Asynchronous", &ui_data.window().asynchronous_simulation);
        ImGui::End();
    }

} // namespace FluidStudio
