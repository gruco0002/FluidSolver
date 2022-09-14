#include "ComponentSettingsWindow.hpp"

#include "ComponentPanelWindow.hpp"
#include "ImguiHelper.hpp"
#include "fluidSolver/IISPHFluidSolver.hpp"
#include "fluidSolver/IISPHFluidSolver3D.hpp"
#include "fluidSolver/SESPHFluidSolver.hpp"
#include "fluidSolver/SESPHFluidSolver3D.hpp"
#include "sensors/IisphSensor.hpp"
#include "time/ConstantTimestepGenerator.hpp"
#include "time/DynamicCflTimestepGenerator.hpp"

#include "SensorGraphWindows.hpp"
#include "visualizer/ContinousVisualizer.hpp"
#include "visualizer/GLParticleRenderer.hpp"
#include "visualizer/GLParticleRenderer3D.hpp"

#include "userInterface/StyledImGuiElements.hpp"


namespace FluidStudio {


    void ComponentSettingsWindow::update() {
        if (ImGui::Begin("Properties")) {
            auto& selection = ui_data.collection().get<ComponentPanelWindow>().selection();
            if (selection.kind != Component::Kind::None) {
                update_component_settings(selection);
            }
        }
        ImGui::End();
    }

    void ComponentSettingsWindow::update_component_settings(const Component& component) {
        if (component.kind == Component::Kind::Solver) {
            update_solver_component();
        } else if (component.kind == Component::Kind::Timestep) {
            update_timestep_component();
        } else if (component.kind == Component::Kind::Sensor) {
            sensor_settings_element.update();
        } else if (component.kind == Component::Kind::Output) {
            update_output_component();
        } else if (component.kind == Component::Kind::Visualizer) {
            update_visualizer_component();
        } else if (component.kind == Component::Kind::Entity) {
            update_entity_component(component.index);
        }
    }

    void ComponentSettingsWindow::update_solver_component() {
        if (StyledImGuiElements::slim_tree_node("Solver Setup", (void*)0x37af7de)) {
            bool can_change = ui_data.window().is_safe_to_access_simulation_data();

            if (ImGui::BeginCombo("Solver", ui_data.window().current_type->name_solver.c_str())) {
                for (auto& name : ui_data.window().solver_types.names_solver) {
                    if (ImGui::Selectable(name.c_str(), ui_data.window().current_type->name_solver == name)) {
                        FluidSolverTypes::FluidSolverTypeQuery q;
                        q.name_solver = name;
                        q.name_neighborhood_search = ui_data.window().current_type->name_neighborhood_search;
                        q.name_kernel = ui_data.window().current_type->name_kernel;
                        auto t = ui_data.window().solver_types.query_type(q, FluidSolverTypes::QueryMainFocus::Solver);
                        if (t && can_change) {
                            ui_data.window().current_type = t;

                            // set the new type
                            ui_data.window().simulator_visualizer_bundle.simulator->data.fluid_solver = t->create_type();
                            ui_data.window().simulator_visualizer_bundle.simulator->data.notify_that_data_changed();
                        }
                    }
                }
                ImGui::EndCombo();
            }

            if (ImGui::BeginCombo("N. Search", ui_data.window().current_type->name_neighborhood_search.c_str())) {
                for (auto& name : ui_data.window().solver_types.names_neighborhood_search) {
                    if (ImGui::Selectable(name.c_str(), ui_data.window().current_type->name_neighborhood_search == name)) {
                        FluidSolverTypes::FluidSolverTypeQuery q;
                        q.name_solver = ui_data.window().current_type->name_solver;
                        q.name_neighborhood_search = name;
                        q.name_kernel = ui_data.window().current_type->name_kernel;
                        auto t =
                                ui_data.window().solver_types.query_type(q, FluidSolverTypes::QueryMainFocus::NeighborhoodSearch);
                        if (t && can_change) {
                            ui_data.window().current_type = t;

                            // set the new type
                            ui_data.window().simulator_visualizer_bundle.simulator->data.fluid_solver = t->create_type();
                            ui_data.window().simulator_visualizer_bundle.simulator->data.notify_that_data_changed();
                        }
                    }
                }
                ImGui::EndCombo();
            }

            if (ImGui::BeginCombo("Kernel", ui_data.window().current_type->name_kernel.c_str())) {
                for (auto& name : ui_data.window().solver_types.names_kernel) {
                    if (ImGui::Selectable(name.c_str(), ui_data.window().current_type->name_kernel == name)) {
                        FluidSolverTypes::FluidSolverTypeQuery q;
                        q.name_solver = ui_data.window().current_type->name_solver;
                        q.name_neighborhood_search = ui_data.window().current_type->name_neighborhood_search;
                        q.name_kernel = name;
                        auto t = ui_data.window().solver_types.query_type(q, FluidSolverTypes::QueryMainFocus::Kernel);
                        if (t && can_change) {
                            ui_data.window().current_type = t;

                            // set the new type
                            ui_data.window().simulator_visualizer_bundle.simulator->data.fluid_solver = t->create_type();
                            ui_data.window().simulator_visualizer_bundle.simulator->data.notify_that_data_changed();
                        }
                    }
                }
                ImGui::EndCombo();
            }

            if (!can_change) {
                ImGui::TextColored(ImColor(0.8f, 0.1f, 0.1f), "Config change not possible");
                ImGui::TextWrapped("Since you are running the simulation in asynchronous mode, you cannot change the "
                                   "solver setup if the simulation is running or has not stopped running!");
            }


            ImGui::TreePop();
        }


        if (ui_data.window().current_type->settings_type == FluidSolverTypes::SolverSettingsTypeSESPH) {
            if (StyledImGuiElements::slim_tree_node("SESPH")) {
                auto v = (LibFluid::SESPHSettings*)ui_data.window().current_type->get_settings(
                        ui_data.window().simulator_visualizer_bundle.simulator->data.fluid_solver);
                update_solver_parameters();
                ImGui::Separator();
                if (ImGui::InputFloat("Viscosity", &v->Viscosity)) {
                    v->notify_that_data_changed();
                }
                if (ImGui::InputFloat("Stiffness", &v->StiffnessK)) {
                    v->notify_that_data_changed();
                }

                ImGui::TreePop();
            }
        }

        if (ui_data.window().current_type->settings_type == FluidSolverTypes::SolverSettingsTypeIISPH) {
            if (StyledImGuiElements::slim_tree_node("IISPH")) {
                update_solver_parameters();
                ImGui::Separator();

                auto v = (LibFluid::IISPHSettings*)ui_data.window().current_type->get_settings(
                        ui_data.window().simulator_visualizer_bundle.simulator->data.fluid_solver);

                if (ImGui::InputFloat("Viscosity", &v->Viscosity)) {
                    v->notify_that_data_changed();
                }
                if (ImGui::InputFloat("Max. Density Err.", &v->MaxDensityErrorAllowed)) {
                    v->notify_that_data_changed();
                }
                if (ImGui::InputInt("Min. Iterations", (int*)&v->MinNumberOfIterations)) {
                    v->notify_that_data_changed();
                }
                if (ImGui::InputInt("Max. Iterations", (int*)&v->MaxNumberOfIterations)) {
                    v->notify_that_data_changed();
                }
                if (ImGui::InputFloat("Gamma", &v->Gamma)) {
                    v->notify_that_data_changed();
                }
                if (ImGui::InputFloat("Omega", &v->Omega)) {
                    v->notify_that_data_changed();
                }
                ImGui::TreePop();
            }
        }

        if (ui_data.window().current_type->settings_type == FluidSolverTypes::SolverSettingsTypeSESPH3D) {
            if (StyledImGuiElements::slim_tree_node("3D SESPH")) {
                auto v = (LibFluid::SESPHSettings3D*)ui_data.window().current_type->get_settings(
                        ui_data.window().simulator_visualizer_bundle.simulator->data.fluid_solver);
                update_solver_parameters();
                ImGui::Separator();
                if (ImGui::InputFloat("Viscosity", &v->Viscosity)) {
                    v->notify_that_data_changed();
                }
                if (ImGui::InputFloat("Stiffness", &v->StiffnessK)) {
                    v->notify_that_data_changed();
                }
                ImGui::Separator();
                if (ImGui::Checkbox("Single-Layer Boundary", &v->single_layer_boundary)) {
                    v->notify_that_data_changed();
                }

                if (v->single_layer_boundary) {
                    if (ImGui::InputFloat("Gamma 1", &v->single_layer_boundary_gamma_1)) {
                        v->notify_that_data_changed();
                    }
                    if (ImGui::InputFloat("Gamma 2", &v->single_layer_boundary_gamma_2)) {
                        v->notify_that_data_changed();
                    }
                }
                ImGui::TreePop();
            }
        }

        if (ui_data.window().current_type->settings_type == FluidSolverTypes::SolverSettingsTypeIISPH3D) {
            if (StyledImGuiElements::slim_tree_node("3D IISPH")) {
                update_solver_parameters();
                ImGui::Separator();

                auto v = (LibFluid::IISPHSettings3D*)ui_data.window().current_type->get_settings(
                        ui_data.window().simulator_visualizer_bundle.simulator->data.fluid_solver);

                if (ImGui::InputFloat("Viscosity", &v->viscosity)) {
                    v->notify_that_data_changed();
                }
                if (ImGui::InputFloat("Max. Density Err.", &v->max_density_error_allowed)) {
                    v->notify_that_data_changed();
                }
                if (ImGui::InputInt("Min. Iterations", (int*)&v->min_number_of_iterations)) {
                    v->notify_that_data_changed();
                }
                if (ImGui::InputInt("Max. Iterations", (int*)&v->max_number_of_iterations)) {
                    v->notify_that_data_changed();
                }
                if (ImGui::InputFloat("Gamma", &v->gamma)) {
                    v->notify_that_data_changed();
                }
                if (ImGui::InputFloat("Omega", &v->omega)) {
                    v->notify_that_data_changed();
                }

                ImGui::Separator();
                if (ImGui::Checkbox("Single-Layer Boundary", &v->single_layer_boundary)) {
                    v->notify_that_data_changed();
                }

                if (v->single_layer_boundary) {
                    if (ImGui::InputFloat("Gamma 1", &v->single_layer_boundary_gamma_1)) {
                        v->notify_that_data_changed();
                    }
                    // TODO: eventually gamma should be in here
                }
                ImGui::TreePop();
            }
        }
    }

    void ComponentSettingsWindow::update_solver_parameters() {
        if (ImGui::InputFloat("Gravity", &ui_data.window().simulator_visualizer_bundle.simulator->parameters.gravity)) {
            ui_data.window().simulator_visualizer_bundle.simulator->parameters.notify_that_data_changed();
        }
        if (ImGui::InputFloat("Rest Density", &ui_data.window().simulator_visualizer_bundle.simulator->parameters.rest_density)) {
            ui_data.window().simulator_visualizer_bundle.simulator->parameters.notify_that_data_changed();
        }
        if (ImGui::InputFloat("Particle Size", &ui_data.window().simulator_visualizer_bundle.simulator->parameters.particle_size)) {
            ui_data.window().simulator_visualizer_bundle.simulator->parameters.notify_that_data_changed();
        }
    }


    void ComponentSettingsWindow::update_entity_component(size_t index) {
        FLUID_ASSERT(index < ui_data.window().simulator_visualizer_bundle.simulator->data.entities.size());
        auto ent = ui_data.window().simulator_visualizer_bundle.simulator->data.entities[index];
        if (StyledImGuiElements::slim_tree_node("Entity")) {
            ImGui::LabelText("Type", Component::get_entity_type_name(ent));

            ImGui::TreePop();
        }

        if (std::dynamic_pointer_cast<LibFluid::ParticleRemover3D>(ent)) {
            update_particle_remover_3d_component(std::dynamic_pointer_cast<LibFluid::ParticleRemover3D>(ent));
        }
    }


    void ComponentSettingsWindow::update_particle_remover_3d_component(std::shared_ptr<LibFluid::ParticleRemover3D> ent) {
        if (StyledImGuiElements::slim_tree_node("Volume")) {
            ImGui::InputFloat3("Center", reinterpret_cast<float*>(&ent->parameters.volume.center));
            ImGui::InputFloat3("Distance", reinterpret_cast<float*>(&ent->parameters.volume.distance_from_center));

            ImGui::TreePop();
        }

        if (StyledImGuiElements::slim_tree_node("Settings")) {
            ImGui::Checkbox("Remove if Outside", &ent->parameters.remove_if_outside);

            ImGui::TreePop();
        }
    }


    void ComponentSettingsWindow::update_timestep_component() {
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


    void ComponentSettingsWindow::update_output_component() {
        auto& output = ui_data.window().simulator_visualizer_bundle.simulator->output;

        if (StyledImGuiElements::slim_tree_node("Output")) {
            ImGui::InputText("Directory", &output->parameters.output_folder);

            ImGui::TreePop();
        }
    }

    void ComponentSettingsWindow::update_visualizer_component() {
        if (StyledImGuiElements::slim_tree_node("Visualizer")) {
            if (ImGui::Button("Update Visualization")) {
                ui_data.window().visualizer_parameter_changed();
            }


            auto gl = std::dynamic_pointer_cast<LibFluid::GLParticleRenderer>(ui_data.window().simulator_visualizer_bundle.visualizer);
            auto cv = std::dynamic_pointer_cast<LibFluid::ContinousVisualizer>(ui_data.window().simulator_visualizer_bundle.visualizer);
            auto gl3d =
                    std::dynamic_pointer_cast<LibFluid::GLParticleRenderer3D>(ui_data.window().simulator_visualizer_bundle.visualizer);

            if (ImGui::BeginCombo("Type",
                        gl ? "Particle Renderer" : (cv ? "Continous Visualizer" : "Particle Renderer 3d"))) {
                if (ImGui::Selectable("Particle Renderer", gl != nullptr)) {
                    if (gl == nullptr) {
                        gl = nullptr;
                        cv = nullptr;
                        gl3d = nullptr;


                        gl = std::make_shared<LibFluid::GLParticleRenderer>();
                        ui_data.window().simulator_visualizer_bundle.visualizer = gl;
                        ui_data.window().simulator_visualizer_bundle.initialize();
                    }
                }
                if (ImGui::Selectable("Continous Visualizer", cv != nullptr)) {
                    if (cv == nullptr) {
                        cv = nullptr;
                        gl = nullptr;
                        gl3d = nullptr;


                        cv = std::make_shared<LibFluid::ContinousVisualizer>();
                        cv->parameters.render_target.width = 100;
                        cv->parameters.render_target.height = 100;
                        cv->settings.minimum_render_density = ui_data.window().simulator_visualizer_bundle.simulator->parameters.rest_density * 0.5f;
                        ui_data.window().simulator_visualizer_bundle.visualizer = cv;
                        ui_data.window().simulator_visualizer_bundle.initialize();
                    }
                }
                if (ImGui::Selectable("Particle Renderer 3d", gl3d != nullptr)) {
                    if (gl3d == nullptr) {
                        cv = nullptr;
                        gl = nullptr;
                        gl3d = nullptr;


                        gl3d = std::make_shared<LibFluid::GLParticleRenderer3D>();
                        gl3d->parameters.render_target.width = 1920;
                        gl3d->parameters.render_target.height = 1080;
                        ui_data.window().simulator_visualizer_bundle.visualizer = gl3d;
                        ui_data.window().simulator_visualizer_bundle.initialize();
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::TreePop();
        }


        if (ui_data.window().simulator_visualizer_bundle.visualizer == nullptr) {
            return;
        }

        FLUID_ASSERT(ui_data.window().simulator_visualizer_bundle.visualizer != nullptr);

        if (StyledImGuiElements::slim_tree_node("Output")) {
            auto visualizer = ui_data.window().simulator_visualizer_bundle.visualizer;

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
        auto cv = std::dynamic_pointer_cast<LibFluid::ContinousVisualizer>(ui_data.window().simulator_visualizer_bundle.visualizer);
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


    void ComponentSettingsWindow::initialize(const UiData& ui_data) {
        // initialize this element
        UiElement::initialize(ui_data);

        // add ui elements to collection
        ui_data.collection().add(sensor_settings_element);

        // initialize ui elements
        sensor_settings_element.initialize(ui_data);
    }

} // namespace FluidStudio