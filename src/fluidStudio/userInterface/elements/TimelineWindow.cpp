#include "TimelineWindow.hpp"
#include "ImguiHelper.hpp"

namespace FluidStudio {

    void TimelineWindow::update() {
        current_index = ui_data.window().timeline_service.current();
        if (ImGui::Begin("Timeline")) {
            if (ImGui::InputInt("Timestep", &current_index, 1, 1)) {
                if (current_index < 0) {
                    current_index = 0;
                } else if (current_index >= ui_data.window().timeline_service.size()) {
                    current_index = ui_data.window().timeline_service.size() - 1;
                } else {
                    ui_data.window().timeline_service.step_to(current_index);
                    ui_data.window().simulator_visualizer_bundle.initialize();
                    ui_data.window().visualizer_parameter_changed();
                }
            }
            ImGui::PushItemWidth(-1);
            if (ImGui::SliderInt(" ", &current_index, 0, ui_data.window().timeline_service.size() - 1)) {
                if (current_index < 0) {
                    current_index = 0;
                } else if (current_index >= ui_data.window().timeline_service.size()) {
                    current_index = ui_data.window().timeline_service.size() - 1;
                } else {
                    ui_data.window().timeline_service.step_to(current_index);
                    ui_data.window().simulator_visualizer_bundle.initialize();
                    ui_data.window().visualizer_parameter_changed();
                }
            }
            ImGui::PopItemWidth();

            ImGui::Separator();
            if (ui_data.window().simulator_visualizer_bundle.simulator != nullptr) {
                auto timepoint = ui_data.window().simulator_visualizer_bundle.simulator->get_current_timepoint();

                ImGui::Text("Timepoint Information");
                ImGui::Text("Number: %d", (int)timepoint.timestep_number);
                ImGui::Text("Simulation time: %fs", timepoint.simulation_time);
                ImGui::Text("Desired time step: %fs", timepoint.desired_time_step);
                ImGui::Text("Actual time step: %fs", timepoint.actual_time_step);
            }
        }
        ImGui::End();
    }
} // namespace FluidStudio