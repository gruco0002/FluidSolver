#include "TimestepSettingsElement.hpp"

#include "ImguiHelper.hpp"
#include "time/ConstantTimestepGenerator.hpp"
#include "time/DynamicCflTimestepGenerator.hpp"
#include "userInterface/StyledImGuiElements.hpp"


namespace FluidStudio {

    void TimestepSettingsElement::update() {
        if (StyledImGuiElements::slim_tree_node("Timestep")) {
            auto ct = std::dynamic_pointer_cast<LibFluid::ConstantTimestepGenerator>(ui_data.window().simulator_visualizer_bundle.simulator->data.timestep_generator);
            auto dt = std::dynamic_pointer_cast<LibFluid::DynamicCflTimestepGenerator>(ui_data.window().simulator_visualizer_bundle.simulator->data.timestep_generator);

            if (ImGui::BeginCombo("Type", ct ? "Constant" : "Dynamic CFL")) {
                if (ImGui::Selectable("Constant", ct != nullptr)) {
                    if (ct == nullptr) {
                        ct = nullptr;
                        dt = nullptr;

                        ct = std::make_shared<LibFluid::ConstantTimestepGenerator>();
                        ui_data.window().simulator_visualizer_bundle.simulator->data.timestep_generator = ct;
                        ui_data.window().simulator_visualizer_bundle.simulator->data.notify_that_data_changed();
                    }
                }
                if (ImGui::Selectable("Dynamic CFL", dt != nullptr)) {
                    if (dt == nullptr) {
                        ct = nullptr;
                        dt = nullptr;

                        dt = std::make_shared<LibFluid::DynamicCflTimestepGenerator>();
                        ui_data.window().simulator_visualizer_bundle.simulator->data.timestep_generator = dt;
                        ui_data.window().simulator_visualizer_bundle.simulator->data.notify_that_data_changed();
                    }
                }
                ImGui::EndCombo();
            }

            if (ct) {
                if (ImGui::InputFloat("Timestep", &ct->settings.timestep, 0.0f, 0.0f, "%.5f")) {
                    ct->settings.timestep = std::max(ct->settings.timestep, 0.00001f);
                }
                if (ImGui::Button("Reset")) {
                    ct->settings = LibFluid::ConstantTimestepGenerator::ConstantTimestepGeneratorSettings();
                }
            }

            if (dt) {
                if (ImGui::InputFloat("Min. Timestep", &dt->settings.min_timestep, 0.0f, 0.0f, "%.5f")) {
                    dt->settings.min_timestep = std::max(dt->settings.min_timestep, 0.00001f);
                    dt->settings.max_timestep = std::max(dt->settings.min_timestep, dt->settings.max_timestep);
                }
                if (ImGui::InputFloat("Max. Timestep", &dt->settings.max_timestep, 0.0f, 0.0f, "%.5f")) {
                    dt->settings.max_timestep = std::max(dt->settings.max_timestep, 0.00001f);
                    dt->settings.min_timestep = std::min(dt->settings.min_timestep, dt->settings.max_timestep);
                }
                ImGui::InputFloat("CFL Number", &dt->settings.cfl_number);
                if (ImGui::Button("Reset")) {
                    dt->settings = LibFluid::DynamicCflTimestepGenerator::DynamicCflTimestepGeneratorSettings();
                }
            }

            ImGui::TreePop();
        }
    }
} // namespace FluidStudio