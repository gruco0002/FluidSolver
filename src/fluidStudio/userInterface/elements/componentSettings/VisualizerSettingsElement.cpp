#include "VisualizerSettingsElement.hpp"

#include "ImguiHelper.hpp"
#include "userInterface/StyledImGuiElements.hpp"
#include "visualizer/ContinuousVisualizer.hpp"
#include "visualizer/GLParticleRenderer.hpp"
#include "visualizer/GLParticleRenderer3D.hpp"
#include "visualizer/raytracer/FluidRaytracer3D.hpp"


namespace FluidStudio {

    void VisualizerSettingsElement::update() {
        if (StyledImGuiElements::slim_tree_node("Visualizer")) {
            auto gl = std::dynamic_pointer_cast<LibFluid::GLParticleRenderer>(ui_data.window().simulator_visualizer_bundle.visualizer);
            auto cv = std::dynamic_pointer_cast<LibFluid::ContinuousVisualizer>(ui_data.window().simulator_visualizer_bundle.visualizer);
            auto gl3d =
                    std::dynamic_pointer_cast<LibFluid::GLParticleRenderer3D>(ui_data.window().simulator_visualizer_bundle.visualizer);
            auto rt = std::dynamic_pointer_cast<LibFluid::Raytracer::FluidRaytracer3D>(ui_data.window().simulator_visualizer_bundle.visualizer);

            bool none_selected = true;
            auto selected_title = "[None]";
            if (gl) {
                none_selected = false;
                selected_title = "Particle Renderer";
            } else if (cv) {
                none_selected = false;
                selected_title = "Continuous Visualizer";
            } else if (gl3d) {
                none_selected = false;
                selected_title = "Particle Renderer 3D";
            } else if (rt) {
                none_selected = false;
                selected_title = "Raytracer";
            }

            if (ImGui::BeginCombo("Type", selected_title)) {
                if (ImGui::Selectable("[None]", none_selected)) {
                    gl = nullptr;
                    cv = nullptr;
                    gl3d = nullptr;
                    rt = nullptr;

                    ui_data.window().simulator_visualizer_bundle.visualizer = nullptr;
                    ui_data.window().simulator_visualizer_bundle.initialize();
                }
                if (ImGui::Selectable("Particle Renderer", gl != nullptr)) {
                    if (gl == nullptr) {
                        gl = nullptr;
                        cv = nullptr;
                        gl3d = nullptr;
                        rt = nullptr;


                        gl = std::make_shared<LibFluid::GLParticleRenderer>();
                        ui_data.window().simulator_visualizer_bundle.visualizer = gl;
                        ui_data.window().simulator_visualizer_bundle.initialize();
                    }
                }
                if (ImGui::Selectable("Continuous Visualizer", cv != nullptr)) {
                    if (cv == nullptr) {
                        cv = nullptr;
                        gl = nullptr;
                        gl3d = nullptr;
                        rt = nullptr;


                        cv = std::make_shared<LibFluid::ContinuousVisualizer>();
                        cv->parameters.render_target.width = 100;
                        cv->parameters.render_target.height = 100;
                        cv->settings.minimum_render_density = ui_data.window().simulator_visualizer_bundle.simulator->parameters.rest_density * 0.5f;
                        ui_data.window().simulator_visualizer_bundle.visualizer = cv;
                        ui_data.window().simulator_visualizer_bundle.initialize();
                    }
                }
                if (ImGui::Selectable("Particle Renderer 3D", gl3d != nullptr)) {
                    if (gl3d == nullptr) {
                        cv = nullptr;
                        gl = nullptr;
                        gl3d = nullptr;
                        rt = nullptr;


                        gl3d = std::make_shared<LibFluid::GLParticleRenderer3D>();
                        gl3d->parameters.render_target.width = 1920;
                        gl3d->parameters.render_target.height = 1080;
                        ui_data.window().simulator_visualizer_bundle.visualizer = gl3d;
                        ui_data.window().simulator_visualizer_bundle.initialize();
                    }
                }

                if (ImGui::Selectable("Raytracer", rt != nullptr)) {
                    cv = nullptr;
                    gl = nullptr;
                    gl3d = nullptr;
                    rt = nullptr;

                    rt = std::make_shared<LibFluid::Raytracer::FluidRaytracer3D>();
                    rt->parameters.render_target.width = 100;
                    rt->parameters.render_target.height = 100;
                    ui_data.window().simulator_visualizer_bundle.visualizer = rt;
                    ui_data.window().simulator_visualizer_bundle.initialize();
                }
                ImGui::EndCombo();
            }

            ImGui::TreePop();
        }


        if (ui_data.window().simulator_visualizer_bundle.visualizer == nullptr) {
            return;
        }

        FLUID_ASSERT(ui_data.window().simulator_visualizer_bundle.visualizer != nullptr);

        if (StyledImGuiElements::slim_tree_node("General")) {
            auto visualizer = ui_data.window().simulator_visualizer_bundle.visualizer;

            if (ImGui::Checkbox("Enabled", &visualizer->parameters.enabled)) {
                visualizer->parameters.notify_that_data_changed();
                ui_data.window().visualizer_parameter_changed();
            }

            if (ImGui::Button("Force Rerender")) {
                ui_data.window().visualizer_parameter_changed();
            }

            if (ImGui::InputInt2("Render Target", (int*)&visualizer->parameters.render_target)) {
                if (visualizer->parameters.render_target.width == 0 || visualizer->parameters.render_target.width == -1)
                    visualizer->parameters.render_target.width = 1;
                if (visualizer->parameters.render_target.height == 0 || visualizer->parameters.render_target.height == -1)
                    visualizer->parameters.render_target.height = 1;
                visualizer->parameters.notify_that_data_changed();
            }

            if (ImGui::Button("Save Image")) {
                auto data = ui_data.window().simulator_visualizer_bundle.visualizer->get_image_data();
                data.save_as_png("visualizer-output.png");
            }

            ImGui::TreePop();
        }

        auto gl = std::dynamic_pointer_cast<LibFluid::GLParticleRenderer>(ui_data.window().simulator_visualizer_bundle.visualizer);
        auto cv = std::dynamic_pointer_cast<LibFluid::ContinuousVisualizer>(ui_data.window().simulator_visualizer_bundle.visualizer);
        auto gl3d = std::dynamic_pointer_cast<LibFluid::GLParticleRenderer3D>(ui_data.window().simulator_visualizer_bundle.visualizer);

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

            if (cv != nullptr) {
                ImGui::ColorEdit4("Background", (float*)&cv->settings.clear_color, ImGuiColorEditFlags_Uint8);
                ImGui::InputFloat("Min. render density", &cv->settings.minimum_render_density);
            }

            if (gl3d != nullptr) {
                ImGui::ColorEdit4("Background", (float*)&gl3d->settings.background_color, ImGuiColorEditFlags_Uint8);
                ImGui::ColorEdit4("Fluid", (float*)&gl3d->settings.fluid_particle_color, ImGuiColorEditFlags_Uint8);
                ImGui::ColorEdit4("Boundary", (float*)&gl3d->settings.boundary_particle_color, ImGuiColorEditFlags_Uint8);

                ImGui::Separator();

                ImGui::Checkbox("Memory Location", &gl3d->settings.show_particle_memory_location);
            }

            ImGui::TreePop();
        }

        if (StyledImGuiElements::slim_tree_node("View")) {
            if (gl != nullptr) {
                ImGui::InputFloat4("Viewport", (float*)&gl->settings.viewport);
            }

            if (cv != nullptr) {
                ImGui::InputFloat4("Viewport", (float*)&cv->settings.viewport);
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