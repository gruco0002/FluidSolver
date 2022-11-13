#include "VisualizerSettingsElement.hpp"

#include "ImguiHelper.hpp"
#include "userInterface/StyledImGuiElements.hpp"
#include "userInterface/helpers/FileDialogHelper.hpp"
#include "visualizer/ContinuousVisualizer.hpp"
#include "visualizer/GLParticleRenderer.hpp"
#include "visualizer/GLParticleRenderer3D.hpp"
#include "visualizer/raytracer/FluidRaytracer3D.hpp"


namespace FluidStudio {

    void VisualizerSettingsElement::update() {
        // visualizer type selectionÏ
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

        // general properties of all visualizers
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
                auto filepath = FileDialogHelper::show_safe_file_dialog("png");
                if (filepath.has_value()) {
                    auto data = ui_data.window().simulator_visualizer_bundle.visualizer->get_image_data();
                    data.save_as_png(filepath.value());
                }
            }

            ImGui::TreePop();
        }

        // update all the different types
        update_gl_renderer();
        update_gl_renderer_3d();
        update_continuous_visualizer();
        update_raytracer();
    }

    void VisualizerSettingsElement::update_gl_renderer() {
        auto gl = std::dynamic_pointer_cast<LibFluid::GLParticleRenderer>(ui_data.window().simulator_visualizer_bundle.visualizer);
        if (gl == nullptr) {
            return;
        }


        if (StyledImGuiElements::slim_tree_node("Colors")) {
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


            ImGui::TreePop();
        }

        if (StyledImGuiElements::slim_tree_node("View")) {
            ImGui::InputFloat4("Viewport", (float*)&gl->settings.viewport);


            ImGui::TreePop();
        }
    }

    void VisualizerSettingsElement::update_gl_renderer_3d() {
        auto gl3d = std::dynamic_pointer_cast<LibFluid::GLParticleRenderer3D>(ui_data.window().simulator_visualizer_bundle.visualizer);
        if (gl3d == nullptr) {
            return;
        }


        if (StyledImGuiElements::slim_tree_node("Colors")) {
            ImGui::ColorEdit4("Background", (float*)&gl3d->settings.background_color, ImGuiColorEditFlags_Uint8);
            ImGui::ColorEdit4("Fluid", (float*)&gl3d->settings.fluid_particle_color, ImGuiColorEditFlags_Uint8);
            ImGui::ColorEdit4("Boundary", (float*)&gl3d->settings.boundary_particle_color, ImGuiColorEditFlags_Uint8);

            ImGui::Separator();

            ImGui::Checkbox("Memory Location", &gl3d->settings.show_particle_memory_location);

            ImGui::TreePop();
        }

        if (StyledImGuiElements::slim_tree_node("View")) {
            if (ImGui::InputFloat3("Camera Location", (float*)&gl3d->settings.camera.location)) {
                ui_data.window().visualizer_parameter_changed();
            }
            if (ImGui::InputFloat3("Looking at", (float*)&gl3d->settings.camera.looking_at)) {
                ui_data.window().visualizer_parameter_changed();
            }
            if (ImGui::InputFloat3("Up Vector", (float*)&gl3d->settings.camera.up)) {
                ui_data.window().visualizer_parameter_changed();
            }

            ImGui::TreePop();
        }
    }

    void VisualizerSettingsElement::update_continuous_visualizer() {
        auto cv = std::dynamic_pointer_cast<LibFluid::ContinuousVisualizer>(ui_data.window().simulator_visualizer_bundle.visualizer);
        if (cv == nullptr) {
            return;
        }

        if (StyledImGuiElements::slim_tree_node("Colors")) {
            ImGui::ColorEdit4("Background", (float*)&cv->settings.clear_color, ImGuiColorEditFlags_Uint8);
            ImGui::InputFloat("Min. render density", &cv->settings.minimum_render_density);

            ImGui::TreePop();
        }

        if (StyledImGuiElements::slim_tree_node("View")) {
            ImGui::InputFloat4("Viewport", (float*)&cv->settings.viewport);

            ImGui::TreePop();
        }
    }

    void VisualizerSettingsElement::update_raytracer() {
        auto rt = std::dynamic_pointer_cast<LibFluid::Raytracer::FluidRaytracer3D>(ui_data.window().simulator_visualizer_bundle.visualizer);
        if (rt == nullptr) {
            return;
        }


        if (StyledImGuiElements::slim_tree_node("Camera")) {
            ImGui::InputFloat3("Position", reinterpret_cast<float*>(&rt->camera.settings.position));
            ImGui::InputFloat3("View direction", reinterpret_cast<float*>(&rt->camera.settings.view_direction));
            ImGui::InputFloat3("Up Vector", reinterpret_cast<float*>(&rt->camera.settings.view_up));
            ImGui::InputFloat("Field of View (Radians)", &rt->camera.settings.field_of_view_x);

            ImGui::Separator();

            ImGui::Checkbox("Flip-Y", &rt->camera.settings.flip_y);
            ImGui::InputInt("Samples per Pixel", reinterpret_cast<int*>(&rt->camera.sample_settings.amount_of_samples));

            ImGui::TreePop();
        }

        if (StyledImGuiElements::slim_tree_node("Tone Mapper")) {
            if (ImGui::Button("Save as HDR")) {
                auto filepath = FileDialogHelper::show_safe_file_dialog("hdr");
                if (filepath.has_value()) {
                    auto image = rt->render_target.as_hdr_image();
                    image.save_as_hdr(filepath.value());
                }
            }
            ImGui::Separator();

            ImGui::InputFloat("Exposure", &rt->tone_mapper.settings.exposure);
            ImGui::InputFloat("Gamma", &rt->tone_mapper.settings.gamma);
            ImGui::Checkbox("Gamma Correction Enabled", &rt->tone_mapper.settings.gamma_correction_enabled);

            if (ImGui::BeginCombo("Mapping", rt->tone_mapper.settings.tone_mapper_function == LibFluid::Raytracer::ToneMapper::ToneMapperSettings::ToneMapperFunction::Exponential ? "Exponential" : "Filmic")) {
                if (ImGui::Selectable("Exponential", rt->tone_mapper.settings.tone_mapper_function == LibFluid::Raytracer::ToneMapper::ToneMapperSettings::ToneMapperFunction::Exponential)) {
                    rt->tone_mapper.settings.tone_mapper_function = LibFluid::Raytracer::ToneMapper::ToneMapperSettings::ToneMapperFunction::Exponential;
                }

                if (ImGui::Selectable("Filmic", rt->tone_mapper.settings.tone_mapper_function == LibFluid::Raytracer::ToneMapper::ToneMapperSettings::ToneMapperFunction::Filmic)) {
                    rt->tone_mapper.settings.tone_mapper_function = LibFluid::Raytracer::ToneMapper::ToneMapperSettings::ToneMapperFunction::Filmic;
                }

                ImGui::EndCombo();
            }

            ImGui::TreePop();
        }

        if (StyledImGuiElements::slim_tree_node("Surface Properties")) {
            ImGui::InputFloat("Fraction of Rest Density", &rt->accelerator.surface_density_as_fraction_of_rest_density);

            ImGui::TreePop();
        }

        if (StyledImGuiElements::slim_tree_node("Skybox")) {
            if (ImGui::Button("Load Image")) {
                auto filepath = FileDialogHelper::show_open_file_dialog("hdr");
                if (filepath.has_value()) {
                    rt->skybox.skybox_image.load_from_hdr(filepath.value());
                }
            }

            if (ImGui::Button("Remove Image")) {
                rt->skybox.skybox_image = LibFluid::Raytracer::HdrImage();
            }

            ImGui::TreePop();
        }
    }
} // namespace FluidStudio