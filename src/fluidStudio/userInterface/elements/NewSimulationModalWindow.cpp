#include "NewSimulationModalWindow.hpp"

#include "ImguiHelper.hpp"
#include "LibFluidMath.hpp"

namespace FluidStudio {

    void NewSimulationModalWindow::update() {
        if (ImGui::BeginPopupModal("Create New Simulation")) {
            ImGui::Text("Creates a new empty %s simulation with the specified parameters.", kind == NewSimulationKind::NewSimulationKind2D ? "2d" : "3d");

            ImGui::Separator();

            if (ImGui::InputFloat("Particle Size (m)", &particle_size)) {
                if (particle_size <= 0.0f) {
                    particle_size = 0.1f;
                }
            }
            if (ImGui::InputFloat("Rest Density (kg/m^3)", &rest_density)) {
                if (rest_density <= 0.0f) {
                    rest_density = 0.1f;
                }
            }
            float particle_mass = LibFluid::Math::pow3(particle_size) * rest_density;

            ImGui::LabelText("Particle Mass (kg)", "%.3f", particle_mass);
            ImGui::Separator();

            if (ImGui::Button("Create Simulation")) {
                if (kind == NewSimulationKind::NewSimulationKind2D) {
                    ui_data.window().create_empty_2d_simulation(particle_size, rest_density);
                } else {
                    ui_data.window().create_empty_3d_simulation(particle_size, rest_density);
                }
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel")) {
                ImGui::CloseCurrentPopup();
            }


            ImGui::EndPopup();
        }


        if (should_open) {
            should_open = false;
            ImGui::OpenPopup("Create New Simulation");
        }
    }

    void NewSimulationModalWindow::open_window(NewSimulationModalWindow::NewSimulationKind kind) {
        reset();

        this->kind = kind;
        should_open = true;
    }
    void NewSimulationModalWindow::reset() {
        rest_density = 1000.0f;
        particle_size = 0.1f;
    }
} // namespace FluidStudio