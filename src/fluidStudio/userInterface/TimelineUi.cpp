#include "TimelineUi.hpp"
#include "FluidSolverWindow.hpp"
#include "ImguiHelper.hpp"

namespace FluidUi {

    void TimelineUi::render() {
        if (window == nullptr)
            return;
        current_index = window->timeline_service.current();
        if (ImGui::Begin("Timeline")) {
            if (ImGui::InputInt("Timestep", &current_index, 1, 1)) {
                if (current_index < 0) {
                    current_index = 0;
                } else if (current_index >= window->timeline_service.size()) {
                    current_index = window->timeline_service.size() - 1;
                } else {
                    window->timeline_service.step_to(current_index);
                    window->simulator_visualizer_bundle.initialize();
                    window->visualizer_parameter_changed();
                }
            }
        }
        ImGui::End();
    }
} // namespace FluidUi