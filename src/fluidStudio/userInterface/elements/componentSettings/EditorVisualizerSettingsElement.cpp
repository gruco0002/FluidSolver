#include "EditorVisualizerSettingsElement.hpp"

#include "ImguiHelper.hpp"
#include "userInterface/StyledImGuiElements.hpp"
#include "userInterface/helpers/FileDialogHelper.hpp"
#include "visualizer/GLParticleRenderer.hpp"
#include "visualizer/GLParticleRenderer3D.hpp"

namespace FluidStudio {

    void EditorVisualizerSettingsElement::update() {
        if (ImGui::Button("Update Visualization")) {
            ui_data.window().visualizer_parameter_changed();
        }

        FLUID_ASSERT(ui_data.window().editor_visualizer != nullptr);

        if (StyledImGuiElements::slim_tree_node("Output")) {
            auto visualizer = std::dynamic_pointer_cast<LibFluid::ISimulationVisualizer>(ui_data.window().editor_visualizer);

            if (ImGui::InputInt2("Render Target", (int*)&visualizer->parameters.render_target)) {
                if (visualizer->parameters.render_target.width == 0 || visualizer->parameters.render_target.width == -1)
                    visualizer->parameters.render_target.width = 1;
                if (visualizer->parameters.render_target.height == 0 || visualizer->parameters.render_target.height == -1)
                    visualizer->parameters.render_target.height = 1;
                visualizer->parameters.notify_that_data_changed();
            }

            if (ImGui::Button("Save Image")) {
                auto filepath = FileDialogHelper::show_safe_file_dialog("png");
                if (filepath.has_value()) {
                    auto data = visualizer->get_image_data();
                    data.save_as_png(filepath.value());
                }
            }

            ImGui::TreePop();
        }

        auto gl = std::dynamic_pointer_cast<LibFluid::GLParticleRenderer>(ui_data.window().editor_visualizer);
        auto gl3d = std::dynamic_pointer_cast<LibFluid::GLParticleRenderer3D>(ui_data.window().editor_visualizer);

        if (StyledImGuiElements::slim_tree_node("Colors")) {
            if (gl != nullptr) {
                ImGui::ColorEdit4("Background", (float*)&gl->settings.backgroundClearColor);
                ImGui::ColorEdit4("Boundary", (float*)&gl->settings.boundaryParticleColor);
                ImGui::Separator();

                static const char* const values[] {"Velocity", "Acceleration", "Mass", "Pressure", "Density"};
                ImGui::Combo("Value", (int*)&gl->settings.colorSelection, values, 5);

                ImGui::InputFloat("Lower Bound", &gl->settings.bottomValue);
                ImGui::ColorEdit4("Lower Color", (float*)&gl->settings.bottomColor);

                ImGui::InputFloat("Upper Bound", &gl->settings.topValue);
                ImGui::ColorEdit4("Upper Color", (float*)&gl->settings.topColor);

                ImGui::Separator();

                ImGui::Checkbox("Memory Location", &gl->settings.showMemoryLocation);
            }


            if (gl3d != nullptr) {
                ImGui::ColorEdit4("Background", (float*)&gl3d->settings.background_color, ImGuiColorEditFlags_Uint8);
                ImGui::ColorEdit4("Fluid", (float*)&gl3d->settings.fluid_particle_color, ImGuiColorEditFlags_Uint8);
                ImGui::ColorEdit4("Boundary", (float*)&gl3d->settings.boundary_particle_color, ImGuiColorEditFlags_Uint8);

                ImGui::Separator();

                ImGui::SliderFloat("Ambient Factor", &gl3d->settings.ambient_light_factor, 0.0f, 1.0f);

                ImGui::Separator();

                ImGui::Checkbox("Memory Location", &gl3d->settings.show_particle_memory_location);
            }

            ImGui::TreePop();
        }

        if (StyledImGuiElements::slim_tree_node("View")) {
            if (ImGui::Button("Transfer View to Simulation Visualizer")) {
                glm::vec3 position(0.0f), view_direction(0.0f), view_up(0.0f);
                ui_data.window().editor_visualizer->get_view(position, view_direction, view_up);
                if (ui_data.window().simulator_visualizer_bundle.visualizer != nullptr) {
                    ui_data.window().simulator_visualizer_bundle.visualizer->set_view(position, view_direction, view_up);
                }
            }

            if (gl != nullptr) {
                ImGui::InputFloat4("Viewport", (float*)&gl->settings.viewport);
            }

            if (gl3d != nullptr) {
                if (ImGui::InputFloat3("Camera Location", (float*)&gl3d->settings.camera.location)) {
                    ui_data.window().visualizer_parameter_changed();
                }
                if (ImGui::InputFloat3("Looking at", (float*)&gl3d->settings.camera.looking_at)) {
                    ui_data.window().visualizer_parameter_changed();
                }
                if (ImGui::InputFloat3("Up Vector", (float*)&gl3d->settings.camera.up)) {
                    ui_data.window().visualizer_parameter_changed();
                }
            }

            ImGui::TreePop();
        }
    }

} // namespace FluidStudio