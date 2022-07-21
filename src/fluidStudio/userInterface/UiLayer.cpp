#include "UiLayer.hpp"

#include "ExtendedSimulationSerializer.hpp"
#include "FluidSolverWindow.hpp"
#include "ImguiHelper.hpp"
#include "entities/ParticleRemover3D.hpp"
#include "fluidSolver/IISPHFluidSolver.hpp"
#include "fluidSolver/IISPHFluidSolver3D.hpp"
#include "fluidSolver/SESPHFluidSolver.hpp"
#include "fluidSolver/SESPHFluidSolver3D.hpp"
#include "sensors/CompressedNeighborsStatistics.hpp"
#include "sensors/IisphSensor.hpp"
#include "time/ConstantTimestepGenerator.hpp"
#include "time/DynamicCflTimestepGenerator.hpp"
#include "userInterface/PlyImport.hpp"
#include "visualizationOverlay/ParticleRemoverOverlay.hpp"
#include "visualizer/ContinousVisualizer.hpp"
#include "visualizer/GLParticleRenderer.hpp"
#include "visualizer/GLParticleRenderer3D.hpp"

#include <filesystem>
#include <functional>
#include <nfd.h>
#include <string>


static void BeginSubsection(const char* name, const std::function<void()>& fnc, void* ptr_id = nullptr) {
    const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed |
            ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap |
            ImGuiTreeNodeFlags_FramePadding;

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2 {4, 4});
    ImGui::Separator();
    if (ptr_id == nullptr)
        ptr_id = (void*)name;
    bool open = ImGui::TreeNodeEx(ptr_id, treeNodeFlags, name);
    ImGui::PopStyleVar();

    if (open) {
        fnc();
        ImGui::TreePop();
    }
}

namespace FluidUi {

    void UiLayer::render_component_panel() {
        if (ImGui::Begin("Components")) {
            // Draw components
            render_component_node("Solver", {Component::Kind::Solver, 0});
            render_component_node("Visualizer", {Component::Kind::Visualizer, 0});
            render_component_node("Timestep", {Component::Kind::Timestep, 0});
            render_component_node("Output", {Component::Kind::Output, 0});

            for (size_t i = 0; i < window->simulator_visualizer_bundle.simulator->data.sensors.size(); i++) {
                auto sen = window->simulator_visualizer_bundle.simulator->data.sensors[i];
                render_component_node(sen->parameters.name.c_str(), {Component::Kind::Sensor, i});
            }

            menu_entity_names.resize(window->simulator_visualizer_bundle.simulator->data.entities.size());
            for (size_t i = 0; i < window->simulator_visualizer_bundle.simulator->data.entities.size(); i++) {
                auto ent = window->simulator_visualizer_bundle.simulator->data.entities[i];
                auto name = "Entity " + std::to_string(i + 1) + ": " + get_entity_type_name(ent);
                if (menu_entity_names[i] != name) {
                    menu_entity_names[i] = name;
                }
                render_component_node(menu_entity_names[i].c_str(), {Component::Kind::Entity, i});
            }


            // handle mouse click outside
            if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
                selection = {};

            // right click outside
            bool is_safe = window->is_safe_to_access_simulation_data();
            if (ImGui::BeginPopupContextWindow(0, 1, false)) {
                if (ImGui::BeginMenu("Add Sensor")) {
                    auto& data = window->simulator_visualizer_bundle.simulator->data;
                    if (ImGui::MenuItem("Global Density", nullptr, nullptr, is_safe)) {
                        auto sen = std::make_shared<FluidSolver::Sensors::GlobalDensitySensor>();
                        sen->parameters.name = "Sensor " + std::to_string(data.sensors.size() + 1);
                        data.sensors.push_back(sen);
                        data.notify_that_data_changed();
                    }
                    if (ImGui::MenuItem("Global Pressure", nullptr, nullptr, is_safe)) {
                        auto sen = std::make_shared<FluidSolver::Sensors::GlobalPressureSensor>();
                        sen->parameters.name = "Sensor " + std::to_string(data.sensors.size() + 1);
                        data.sensors.push_back(sen);
                        data.notify_that_data_changed();
                    }
                    if (ImGui::MenuItem("Global Velocity", nullptr, nullptr, is_safe)) {
                        auto sen = std::make_shared<FluidSolver::Sensors::GlobalVelocitySensor>();
                        sen->parameters.name = "Sensor " + std::to_string(data.sensors.size() + 1);
                        data.sensors.push_back(sen);
                        data.notify_that_data_changed();
                    }
                    if (ImGui::MenuItem("Global Energy", nullptr, nullptr, is_safe)) {
                        auto sen = std::make_shared<FluidSolver::Sensors::GlobalEnergySensor>();
                        sen->parameters.name = "Sensor " + std::to_string(data.sensors.size() + 1);
                        data.sensors.push_back(sen);
                        data.notify_that_data_changed();
                    }
                    if (ImGui::MenuItem("Global Particle Count", nullptr, nullptr, is_safe)) {
                        auto sen = std::make_shared<FluidSolver::Sensors::GlobalParticleCountSensor>();
                        sen->parameters.name = "Sensor " + std::to_string(data.sensors.size() + 1);
                        data.sensors.push_back(sen);
                        data.notify_that_data_changed();
                    }
                    if (ImGui::MenuItem("3D Sensor Plane", nullptr, nullptr, is_safe)) {
                        auto sen = std::make_shared<FluidSolver::Sensors::SensorPlane>();
                        sen->parameters.name = "Sensor " + std::to_string(data.sensors.size() + 1);
                        data.sensors.push_back(sen);
                        data.notify_that_data_changed();
                    }
                    if (ImGui::MenuItem("Compressed Neighborhood Storage", nullptr, nullptr, is_safe)) {
                        auto sen = std::make_shared<FluidSolver::Sensors::CompressedNeighborStorageSensor>();
                        sen->parameters.name = "Sensor " + std::to_string(data.sensors.size() + 1);
                        data.sensors.push_back(sen);
                        data.notify_that_data_changed();
                    }
                    if (ImGui::MenuItem("IISPH", nullptr, nullptr, is_safe)) {
                        auto sen = std::make_shared<FluidSolver::Sensors::IISPHSensor>();
                        sen->parameters.name = "Sensor " + std::to_string(data.sensors.size() + 1);
                        data.sensors.push_back(sen);
                        data.notify_that_data_changed();
                    }

                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Add Entity")) {
                    auto& data = window->simulator_visualizer_bundle.simulator->data;
                    if (ImGui::MenuItem("Particle Remover 3D", nullptr, nullptr, is_safe)) {
                        auto ent = std::make_shared<FluidSolver::ParticleRemover3D>();
                        ent->parameters.volume.center = glm::vec3(0.0f);
                        ent->parameters.volume.distance_from_center = glm::vec3(1.0f);
                        ent->parameters.remove_if_outside = true;
                        data.entities.push_back(ent);
                        data.notify_that_data_changed();
                    }

                    ImGui::EndMenu();
                }
                ImGui::EndPopup();
            }
        }
        ImGui::End();

        update_selection_based_ui();
    }

    void UiLayer::render_simulation_controls() {
        ImGui::Begin("Simulation Controls");
        if (window->simulation_should_run) {
            ImGui::TextColored(ImColor(0.1f, 0.8f, 0.1f), "Running");
        } else if (window->are_calculations_running()) {
            ImGui::TextColored(ImColor(0.8f, 0.8f, 0.1f), "Paused, pending calculations");
        } else {
            ImGui::TextColored(ImColor(0.8f, 0.1f, 0.1f), "Paused");
        }

        if (ImGui::Button("Run")) {
            this->window->simulation_should_run = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Pause")) {
            this->window->simulation_should_run = false;
        }

        ImGui::Checkbox("Asynchronous", &window->asynchronous_simulation);
        ImGui::End();
    }

    void UiLayer::render_component_node(const char* name, const Component& component) {
        ImGuiTreeNodeFlags flags = ((component == selection) ? ImGuiTreeNodeFlags_Selected : 0) |
                ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth |
                ImGuiTreeNodeFlags_FramePadding;

        bool opened = ImGui::TreeNodeEx((void*)((size_t)name + component.index * 1565412), flags, name);

        if (ImGui::IsItemClicked()) {
            selection = component;
        }


        bool component_deleted = false;

        if (component.can_delete()) {
            if (ImGui::BeginPopupContextItem()) {
                if (ImGui::MenuItem("Delete Component"))
                    component_deleted = true;

                ImGui::EndPopup();
            }
        }

        if (opened) {
            ImGui::TreePop();
        }

        if (component_deleted) {
            // reset selction if required
            if (selection == component)
                selection = {};

            // remove component
            delete_component(component);
        }
    }

    void UiLayer::render_component_settings(const Component& component) {
        if (component.kind == Component::Kind::Solver) {
            render_solver_component();
        } else if (component.kind == Component::Kind::Timestep) {
            render_timestep_component();
        } else if (component.kind == Component::Kind::Sensor) {
            render_sensor_component(component.index);
        } else if (component.kind == Component::Kind::Output) {
            render_output_component();
        } else if (component.kind == Component::Kind::Visualizer) {
            render_visualizer_component();
        } else if (component.kind == Component::Kind::Entity) {
            render_entity_component(component.index);
        }
    }

    void UiLayer::render_solver_component() {
        BeginSubsection(
                "Solver Setup",
                [=]() {
                    bool can_change = window->is_safe_to_access_simulation_data();

                    if (ImGui::BeginCombo("Solver", window->current_type->name_solver.c_str())) {
                        for (auto& name : window->solver_types.names_solver) {
                            if (ImGui::Selectable(name.c_str(), window->current_type->name_solver == name)) {
                                FluidSolverTypes::FluidSolverTypeQuery q;
                                q.name_solver = name;
                                q.name_neighborhood_search = window->current_type->name_neighborhood_search;
                                q.name_kernel = window->current_type->name_kernel;
                                auto t = window->solver_types.query_type(q, FluidSolverTypes::QueryMainFocus::Solver);
                                if (t && can_change) {
                                    window->current_type = t;

                                    // set the new type
                                    window->simulator_visualizer_bundle.simulator->data.fluid_solver = t->create_type();
                                    window->simulator_visualizer_bundle.simulator->data.notify_that_data_changed();
                                }
                            }
                        }
                        ImGui::EndCombo();
                    }

                    if (ImGui::BeginCombo("N. Search", window->current_type->name_neighborhood_search.c_str())) {
                        for (auto& name : window->solver_types.names_neighborhood_search) {
                            if (ImGui::Selectable(name.c_str(), window->current_type->name_neighborhood_search == name)) {
                                FluidSolverTypes::FluidSolverTypeQuery q;
                                q.name_solver = window->current_type->name_solver;
                                q.name_neighborhood_search = name;
                                q.name_kernel = window->current_type->name_kernel;
                                auto t =
                                        window->solver_types.query_type(q, FluidSolverTypes::QueryMainFocus::NeighborhoodSearch);
                                if (t && can_change) {
                                    window->current_type = t;

                                    // set the new type
                                    window->simulator_visualizer_bundle.simulator->data.fluid_solver = t->create_type();
                                    window->simulator_visualizer_bundle.simulator->data.notify_that_data_changed();
                                }
                            }
                        }
                        ImGui::EndCombo();
                    }

                    if (ImGui::BeginCombo("Kernel", window->current_type->name_kernel.c_str())) {
                        for (auto& name : window->solver_types.names_kernel) {
                            if (ImGui::Selectable(name.c_str(), window->current_type->name_kernel == name)) {
                                FluidSolverTypes::FluidSolverTypeQuery q;
                                q.name_solver = window->current_type->name_solver;
                                q.name_neighborhood_search = window->current_type->name_neighborhood_search;
                                q.name_kernel = name;
                                auto t = window->solver_types.query_type(q, FluidSolverTypes::QueryMainFocus::Kernel);
                                if (t && can_change) {
                                    window->current_type = t;

                                    // set the new type
                                    window->simulator_visualizer_bundle.simulator->data.fluid_solver = t->create_type();
                                    window->simulator_visualizer_bundle.simulator->data.notify_that_data_changed();
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
                },
                (void*)0x37af7de);

        if (window->current_type->settings_type == FluidSolverTypes::SolverSettingsTypeSESPH) {
            BeginSubsection("SESPH", [=]() {
                auto v = (FluidSolver::SESPHSettings*)window->current_type->get_settings(
                        window->simulator_visualizer_bundle.simulator->data.fluid_solver);
                render_solver_parameters();
                ImGui::Separator();
                if (ImGui::InputFloat("Viscosity", &v->Viscosity)) {
                    v->notify_that_data_changed();
                }
                if (ImGui::InputFloat("Stiffness", &v->StiffnessK)) {
                    v->notify_that_data_changed();
                }
            });
        }

        if (window->current_type->settings_type == FluidSolverTypes::SolverSettingsTypeIISPH) {
            BeginSubsection("IISPH", [=]() {
                render_solver_parameters();
                ImGui::Separator();

                auto v = (FluidSolver::IISPHSettings*)window->current_type->get_settings(
                        window->simulator_visualizer_bundle.simulator->data.fluid_solver);

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
            });
        }

        if (window->current_type->settings_type == FluidSolverTypes::SolverSettingsTypeSESPH3D) {
            BeginSubsection("3D SESPH", [=]() {
                auto v = (FluidSolver::SESPHSettings3D*)window->current_type->get_settings(
                        window->simulator_visualizer_bundle.simulator->data.fluid_solver);
                render_solver_parameters();
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
            });
        }

        if (window->current_type->settings_type == FluidSolverTypes::SolverSettingsTypeIISPH3D) {
            BeginSubsection("3D IISPH", [=]() {
                render_solver_parameters();
                ImGui::Separator();

                auto v = (FluidSolver::IISPHSettings3D*)window->current_type->get_settings(
                        window->simulator_visualizer_bundle.simulator->data.fluid_solver);

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
                    if (ImGui::InputFloat("Gamma 2", &v->single_layer_boundary_gamma_2)) {
                        v->notify_that_data_changed();
                    }
                }
            });
        }
    }

    void UiLayer::render_timestep_component() {
        BeginSubsection("Timestep", [=]() {
            auto ct = std::dynamic_pointer_cast<FluidSolver::ConstantTimestepGenerator>(window->simulator_visualizer_bundle.simulator->data.timestep_generator);
            auto dt = std::dynamic_pointer_cast<FluidSolver::DynamicCflTimestepGenerator>(window->simulator_visualizer_bundle.simulator->data.timestep_generator);

            if (ImGui::BeginCombo("Type", ct ? "Constant" : "Dynamic CFL")) {
                if (ImGui::Selectable("Constant", ct != nullptr)) {
                    if (ct == nullptr) {
                        ct = nullptr;
                        dt = nullptr;

                        ct = std::make_shared<FluidSolver::ConstantTimestepGenerator>();
                        window->simulator_visualizer_bundle.simulator->data.timestep_generator = ct;
                        window->simulator_visualizer_bundle.simulator->data.notify_that_data_changed();
                    }
                }
                if (ImGui::Selectable("Dynamic CFL", dt != nullptr)) {
                    if (dt == nullptr) {
                        ct = nullptr;
                        dt = nullptr;

                        dt = std::make_shared<FluidSolver::DynamicCflTimestepGenerator>();
                        window->simulator_visualizer_bundle.simulator->data.timestep_generator = dt;
                        window->simulator_visualizer_bundle.simulator->data.notify_that_data_changed();
                    }
                }
                ImGui::EndCombo();
            }

            if (ct) {
                if (ImGui::InputFloat("Timestep", &ct->settings.timestep, 0.0f, 0.0f, "%.5f")) {
                    ct->settings.timestep = std::max(ct->settings.timestep, 0.00001f);
                }
                if (ImGui::Button("Reset")) {
                    ct->settings = FluidSolver::ConstantTimestepGenerator::ConstantTimestepGeneratorSettings();
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
                    dt->settings = FluidSolver::DynamicCflTimestepGenerator::DynamicCflTimestepGeneratorSettings();
                }
            }
        });
    }

    const char* get_sensor_type_name(const std::shared_ptr<FluidSolver::ISensor>& sen) {
        if (std::dynamic_pointer_cast<FluidSolver::Sensors::GlobalDensitySensor>(sen)) {
            return "Global Density";
        } else if (std::dynamic_pointer_cast<FluidSolver::Sensors::GlobalPressureSensor>(sen)) {
            return "Global Pressure";
        } else if (std::dynamic_pointer_cast<FluidSolver::Sensors::GlobalVelocitySensor>(sen)) {
            return "Global Velocity";
        } else if (std::dynamic_pointer_cast<FluidSolver::Sensors::GlobalEnergySensor>(sen)) {
            return "Global Energy";
        } else if (std::dynamic_pointer_cast<FluidSolver::Sensors::GlobalParticleCountSensor>(sen)) {
            return "Global Particle Count";
        } else if (std::dynamic_pointer_cast<FluidSolver::Sensors::SensorPlane>(sen)) {
            return "3D Sensor Plane";
        } else if (std::dynamic_pointer_cast<FluidSolver::Sensors::CompressedNeighborStorageSensor>(sen)) {
            return "Compressed Neighbor Storage";
        } else if (std::dynamic_pointer_cast<FluidSolver::Sensors::IISPHSensor>(sen)) {
            return "IISPH";
        }
        return "UNKNOWN";
    }

    void UiLayer::render_sensor_component(size_t index) {
        FLUID_ASSERT(window != nullptr);
        FLUID_ASSERT(index < window->simulator_visualizer_bundle.simulator->data.sensors.size());
        auto sen = window->simulator_visualizer_bundle.simulator->data.sensors[index];
        BeginSubsection("Sensor", [&]() {
            ImGui::LabelText("Type", get_sensor_type_name(sen));
            if (ImGui::Button("Open Graph")) {
                statisticsUi.open_sensor_window(index);
            }
            ImGui::InputText("Name", &sen->parameters.name);
            ImGui::Checkbox("Save to File", &sen->parameters.save_to_file);
            ImGui::Checkbox("Keep Data after Saving", &sen->parameters.keep_data_in_memory_after_saving);
        });

        if (std::dynamic_pointer_cast<FluidSolver::Sensors::GlobalEnergySensor>(sen)) {
            render_global_energy_sensor_component(std::dynamic_pointer_cast<FluidSolver::Sensors::GlobalEnergySensor>(sen));
        } else if (std::dynamic_pointer_cast<FluidSolver::Sensors::SensorPlane>(sen)) {
            render_sensor_plane_component(std::dynamic_pointer_cast<FluidSolver::Sensors::SensorPlane>(sen));
        }
    }

    void UiLayer::render_output_component() {
        FLUID_ASSERT(window != nullptr);

        auto& output = window->simulator_visualizer_bundle.simulator->output;
        BeginSubsection("Output", [&]() {
            ImGui::InputText("Directory", &output->parameters.output_folder);
            if (ImGui::InputInt("Write Interval", (int*)&output->parameters.timesteps_between_sensor_save)) {
                if (output->parameters.timesteps_between_sensor_save == 0 ||
                        output->parameters.timesteps_between_sensor_save == -1) {
                    output->parameters.timesteps_between_sensor_save = 1;
                }
            }
        });
    }

    void UiLayer::render_visualizer_component() {
        FLUID_ASSERT(window != nullptr);

        BeginSubsection("Visualizer", [&]() {
            if (ImGui::Button("Update Visualization")) {
                window->visualizer_parameter_changed();
            }


            auto gl = std::dynamic_pointer_cast<FluidSolver::GLParticleRenderer>(window->simulator_visualizer_bundle.visualizer);
            auto cv = std::dynamic_pointer_cast<FluidSolver::ContinousVisualizer>(window->simulator_visualizer_bundle.visualizer);
            auto gl3d =
                    std::dynamic_pointer_cast<FluidSolver::GLParticleRenderer3D>(window->simulator_visualizer_bundle.visualizer);

            if (ImGui::BeginCombo("Type",
                        gl ? "Particle Renderer" : (cv ? "Continous Visualizer" : "Particle Renderer 3d"))) {
                if (ImGui::Selectable("Particle Renderer", gl != nullptr)) {
                    if (gl == nullptr) {
                        gl = nullptr;
                        cv = nullptr;
                        gl3d = nullptr;


                        gl = std::make_shared<FluidSolver::GLParticleRenderer>();
                        window->simulator_visualizer_bundle.visualizer = gl;
                        window->simulator_visualizer_bundle.initialize();
                    }
                }
                if (ImGui::Selectable("Continous Visualizer", cv != nullptr)) {
                    if (cv == nullptr) {
                        cv = nullptr;
                        gl = nullptr;
                        gl3d = nullptr;


                        cv = std::make_shared<FluidSolver::ContinousVisualizer>();
                        cv->parameters.render_target.width = 100;
                        cv->parameters.render_target.height = 100;
                        cv->settings.minimum_render_density = window->simulator_visualizer_bundle.simulator->parameters.rest_density * 0.5f;
                        window->simulator_visualizer_bundle.visualizer = cv;
                        window->simulator_visualizer_bundle.initialize();
                    }
                }
                if (ImGui::Selectable("Particle Renderer 3d", gl3d != nullptr)) {
                    if (gl3d == nullptr) {
                        cv = nullptr;
                        gl = nullptr;
                        gl3d = nullptr;


                        gl3d = std::make_shared<FluidSolver::GLParticleRenderer3D>();
                        gl3d->parameters.render_target.width = 1920;
                        gl3d->parameters.render_target.height = 1080;
                        window->simulator_visualizer_bundle.visualizer = gl3d;
                        window->simulator_visualizer_bundle.initialize();
                    }
                }
                ImGui::EndCombo();
            }
        });

        if (window->simulator_visualizer_bundle.visualizer == nullptr) {
            return;
        }

        FLUID_ASSERT(window->simulator_visualizer_bundle.visualizer != nullptr);

        BeginSubsection("Output", [&] {
            auto visualizer = window->simulator_visualizer_bundle.visualizer;

            if (ImGui::InputInt2("Render Target", (int*)&visualizer->parameters.render_target)) {
                if (visualizer->parameters.render_target.width == 0 || visualizer->parameters.render_target.width == -1)
                    visualizer->parameters.render_target.width = 1;
                if (visualizer->parameters.render_target.height == 0 || visualizer->parameters.render_target.height == -1)
                    visualizer->parameters.render_target.height = 1;
                visualizer->parameters.notify_that_data_changed();
            }

            if (ImGui::Button("Save Image")) {
                auto data = window->simulator_visualizer_bundle.visualizer->get_image_data();
                data.save_as_png("visualizer-output.png");
            }
        });


        auto gl = std::dynamic_pointer_cast<FluidSolver::GLParticleRenderer>(window->simulator_visualizer_bundle.visualizer);
        auto cv = std::dynamic_pointer_cast<FluidSolver::ContinousVisualizer>(window->simulator_visualizer_bundle.visualizer);
        auto gl3d = std::dynamic_pointer_cast<FluidSolver::GLParticleRenderer3D>(window->simulator_visualizer_bundle.visualizer);

        BeginSubsection("Colors", [&] {
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
        });

        BeginSubsection("View", [&] {
            if (gl != nullptr) {
                ImGui::InputFloat4("Viewport", (float*)&gl->settings.viewport);
            }

            if (cv != nullptr) {
                ImGui::InputFloat4("Viewport", (float*)&cv->settings.viewport);
            }

            if (gl3d != nullptr) {
                if (ImGui::InputFloat3("Camera Location", (float*)&gl3d->settings.camera.location)) {
                    window->visualizer_parameter_changed();
                }
                if (ImGui::InputFloat3("Looking at", (float*)&gl3d->settings.camera.looking_at)) {
                    window->visualizer_parameter_changed();
                }
                if (ImGui::InputFloat3("Up Vector", (float*)&gl3d->settings.camera.up)) {
                    window->visualizer_parameter_changed();
                }
            }
        });
    }

    void UiLayer::render_global_energy_sensor_component(
            std::shared_ptr<FluidSolver::Sensors::GlobalEnergySensor> sen) {
        BeginSubsection("Zero-Levels", [&]() { ImGui::InputFloat("Zero Height", &sen->settings.relative_zero_height); });
    }

    void UiLayer::delete_component(const Component& component) {
        if (component.kind == Component::Kind::Sensor) {
            // delete the sensor if existing

            // check for existance
            if (window->simulator_visualizer_bundle.simulator->data.sensors.size() <= component.index) {
                return;
            }

            // delete the sensor
            for (size_t i = component.index; i < window->simulator_visualizer_bundle.simulator->data.sensors.size() - 1; i++) {
                window->simulator_visualizer_bundle.simulator->data.sensors[i] = window->simulator_visualizer_bundle.simulator->data.sensors[i + 1];
            }
            window->simulator_visualizer_bundle.simulator->data.sensors.pop_back();
            window->simulator_visualizer_bundle.simulator->data.notify_that_data_changed();
        } else if (component.kind == Component::Kind::Entity) {
            // delete the entity if existing

            // check for existance
            if (window->simulator_visualizer_bundle.simulator->data.entities.size() <= component.index) {
                return;
            }

            // delete the entity
            for (size_t i = component.index; i < window->simulator_visualizer_bundle.simulator->data.entities.size() - 1; i++) {
                window->simulator_visualizer_bundle.simulator->data.entities[i] = window->simulator_visualizer_bundle.simulator->data.entities[i + 1];
            }
            window->simulator_visualizer_bundle.simulator->data.entities.pop_back();
            window->simulator_visualizer_bundle.simulator->data.notify_that_data_changed();
        }
    }

    UiLayer::UiLayer() {
        this->ply_import = std::make_unique<PlyImport>(window);
    }

    UiLayer::~UiLayer() {
        window = nullptr;
    }

    void UiLayer::render() {
        render_menu();
        render_simulation_controls();

        // render component controls
        render_component_panel();
        render_component_properties_panel();

        // render other windows
        statisticsUi.render();
        logWindow.render();

        if (ply_import) {
            ply_import->render();
        }

        timeline_ui.render();
    }

    void UiLayer::initialize() {
        statisticsUi.window = window;
        statisticsUi.initialize();
        logWindow.window = window;
        logWindow.initialize();

        timeline_ui.window = window;

        if (ply_import) {
            ply_import->set_window(window);
        }
    }


    void UiLayer::render_solver_parameters() {
        if (ImGui::InputFloat("Gravity", &window->simulator_visualizer_bundle.simulator->parameters.gravity)) {
            window->simulator_visualizer_bundle.simulator->parameters.notify_that_data_changed();
        }
        if (ImGui::InputFloat("Rest Density", &window->simulator_visualizer_bundle.simulator->parameters.rest_density)) {
            window->simulator_visualizer_bundle.simulator->parameters.notify_that_data_changed();
        }
        if (ImGui::InputFloat("Particle Size", &window->simulator_visualizer_bundle.simulator->parameters.particle_size)) {
            window->simulator_visualizer_bundle.simulator->parameters.notify_that_data_changed();
        }
    }

    void UiLayer::render_menu() {
        static bool save_menu_open = false;


        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                bool can_change = window->is_safe_to_access_simulation_data();

                if (ImGui::MenuItem("New", nullptr, false, can_change)) {
                    // create a new scenario
                    window->create_empty_simulation();
                }

                if (ImGui::MenuItem("Open", nullptr, false, can_change)) {
                    // open scenario

                    char* p = nullptr;
                    auto res = NFD_OpenDialog("yaml", nullptr, &p);
                    if (res == NFD_OKAY) {
                        std::string path(p);
                        free(p);

                        // load simulation
                        ExtendedSimulationSerializer s(path);
                        auto simulation = s.load_from_file();
                        if (!s.has_errors()) {
                            window->simulator_visualizer_bundle = simulation;
                            window->on_new_simulation();
                        }
                    }
                }

                if (ImGui::MenuItem("Save", nullptr, false, can_change)) {
                    save_menu_open = true;
                }

                if (ImGui::BeginMenu("Import", can_change)) {
                    if (ImGui::MenuItem("Ply File", nullptr, false, can_change)) {
                        if (ply_import) {
                            ply_import->show();
                        }
                    }

                    ImGui::EndMenu();
                }


                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Test")) {
                bool can_change = window->is_safe_to_access_simulation_data();

                if (ImGui::MenuItem("Test 3D", nullptr, false, can_change)) {
                    window->create_3d_test_simulation();
                }

                ImGui::EndMenu();
            }


            ImGui::EndMainMenuBar();
        }


        if (ImGui::BeginPopupModal("Save Simulation")) {
            static char* path = nullptr;
            static std::string particle_filepath = "particles.data";
            static bool save_particle_data = true;


            ImGui::TextWrapped(
                    "Save the current simulation as yaml file. Optionally you can save the particle data. If it already exists "
                    "and you do not want to override it, make sure to uncheck the checkbox and provide the name of the current "
                    "particle data file relative to the yaml file in the text field.");

            ImGui::Separator();

            if (ImGui::Button("Choose")) {
                char* p = nullptr;

                auto res = NFD_SaveDialog("yaml", nullptr, &p);
                if (res == NFD_OKAY) {
                    if (path != nullptr)
                        free(path);
                    path = p;
                    particle_filepath = std::filesystem::path(path).filename().replace_extension(".data").string();
                } else {
                    free(p);
                }
            }
            ImGui::SameLine();
            if (path != nullptr) {
                ImGui::LabelText("File", path);
            } else {
                ImGui::LabelText("File", "Not selected");
            }

            ImGui::Separator();
            ImGui::Checkbox("Save Particle Data", &save_particle_data);
            ImGui::InputText("Particle File", &particle_filepath);

            if (ImGui::Button("Close")) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Save") && path != nullptr) {
                // Save
                ExtendedSimulationSerializer s(path, {save_particle_data, particle_filepath});
                s.save_to_file(window->simulator_visualizer_bundle);
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
        if (save_menu_open) {
            save_menu_open = false;
            ImGui::OpenPopup("Save Simulation");
        }
    }

    void UiLayer::render_component_properties_panel() {
        if (ImGui::Begin("Properties")) {
            if (selection.kind != Component::Kind::None) {
                render_component_settings(selection);
            }
        }
        ImGui::End();
    }

    bool UiLayer::Component::operator==(const Component& other) const {
        return kind == other.kind && index == other.index;
    }

    bool UiLayer::Component::operator!=(const Component& other) const {
        return !(*this == other);
    }

    bool UiLayer::Component::can_delete() const {
        switch (kind) {
            case Kind::Sensor:
            case Kind::Entity:
                return true;
        }

        return false;
    }


    void UiLayer::render_sensor_plane_component(std::shared_ptr<FluidSolver::Sensors::SensorPlane> sen) {
        BeginSubsection("Value", [&] {
            using namespace FluidSolver::Sensors;

            const char* current_type_name = nullptr;
            switch (sen->settings.sensor_type) {
                case SensorPlane::SensorPlaneType::SensorPlaneTypeDensity:
                    current_type_name = "Density";
                    break;
                case SensorPlane::SensorPlaneType::SensorPlaneTypeVelocity:
                    current_type_name = "Velocity";
                    break;
                case SensorPlane::SensorPlaneType::SensorPlaneTypePressure:
                    current_type_name = "Pressure";
                    break;
                default:
                    current_type_name = "Unknown";
                    break;
            }

            if (ImGui::BeginCombo("Type", current_type_name)) {
                if (ImGui::Selectable("Density",
                            sen->settings.sensor_type == SensorPlane::SensorPlaneType::SensorPlaneTypeDensity)) {
                    sen->settings.sensor_type = SensorPlane::SensorPlaneType::SensorPlaneTypeDensity;
                }

                if (ImGui::Selectable("Velocity",
                            sen->settings.sensor_type == SensorPlane::SensorPlaneType::SensorPlaneTypeVelocity)) {
                    sen->settings.sensor_type = SensorPlane::SensorPlaneType::SensorPlaneTypeVelocity;
                }

                if (ImGui::Selectable("Pressure",
                            sen->settings.sensor_type == SensorPlane::SensorPlaneType::SensorPlaneTypePressure)) {
                    sen->settings.sensor_type = SensorPlane::SensorPlaneType::SensorPlaneTypePressure;
                }

                ImGui::EndCombo();
            }

            if (ImGui::InputInt("Calc. every nth step", (int*)&sen->settings.calculate_plane_every_nth_step)) {
                if (sen->settings.calculate_plane_every_nth_step == 0) {
                    sen->settings.calculate_plane_every_nth_step = 1;
                }
            }
        });

        BeginSubsection("Location & Size", [&] {
            ImGui::InputFloat3("Origin", (float*)&sen->settings.origin);
            ImGui::InputFloat3("Span X", (float*)&sen->settings.span_x);
            ImGui::InputFloat3("Span Y", (float*)&sen->settings.span_y);
            ImGui::InputFloat("Width", &sen->settings.width);
            ImGui::InputFloat("Height", &sen->settings.height);
        });

        BeginSubsection("Samples", [&] {
            ImGui::InputInt("X-Samples", (int*)&sen->settings.number_of_samples_x);
            ImGui::InputInt("Y-Samples", (int*)&sen->settings.number_of_samples_y);
            ImGui::InputInt("Sub-Samples", (int*)&sen->settings.sub_sample_grid_size);
        });

        BeginSubsection("Image", [&] {
            ImGui::InputFloat("Min. value", &sen->settings.min_image_value);
            ImGui::InputFloat("Max. value", &sen->settings.max_image_value);
        });
    }
    void UiLayer::render_entity_component(size_t index) {
        FLUID_ASSERT(window != nullptr);
        FLUID_ASSERT(index < window->simulator_visualizer_bundle.simulator->data.entities.size());
        auto ent = window->simulator_visualizer_bundle.simulator->data.entities[index];
        BeginSubsection("Entity", [&]() {
            ImGui::LabelText("Type", get_entity_type_name(ent));
        });

        if (std::dynamic_pointer_cast<FluidSolver::ParticleRemover3D>(ent)) {
            render_particle_remover_3d_component(std::dynamic_pointer_cast<FluidSolver::ParticleRemover3D>(ent));
        }
    }
    const char* UiLayer::get_entity_type_name(const std::shared_ptr<FluidSolver::IEntity> entity) const {
        if (std::dynamic_pointer_cast<FluidSolver::ParticleRemover3D>(entity)) {
            return "Particle Remover 3D";
        }
        return "";
    }

    void UiLayer::render_particle_remover_3d_component(std::shared_ptr<FluidSolver::ParticleRemover3D> ent) {
        BeginSubsection("Volume", [&] {
            ImGui::InputFloat3("Center", reinterpret_cast<float*>(&ent->parameters.volume.center));
            ImGui::InputFloat3("Distance", reinterpret_cast<float*>(&ent->parameters.volume.distance_from_center));
        });
        BeginSubsection("Settings", [&] {
            ImGui::Checkbox("Remove if Outside", &ent->parameters.remove_if_outside);
        });
    }

    void UiLayer::update_selection_based_ui() {
        std::shared_ptr<OverlayInstance> new_overlay_instance = nullptr;
        auto current_overlay_instance = window->visualization_overlay.data.overlay_instance;
        if (selection.kind == Component::Kind::Entity) {
            // check if overlay needs to be activated / deactivated
            auto ent = window->simulator_visualizer_bundle.simulator->data.entities[selection.index];

            auto remover3d = std::dynamic_pointer_cast<FluidSolver::ParticleRemover3D>(ent);
            if (remover3d != nullptr) {
                // activate the overlay for the remover 3d
                auto overlay_casted = std::dynamic_pointer_cast<ParticleRemoverOverlay>(current_overlay_instance);
                if (overlay_casted != nullptr && overlay_casted->entity == ent) {
                    new_overlay_instance = current_overlay_instance;
                } else {
                    new_overlay_instance = std::make_shared<ParticleRemoverOverlay>(remover3d);
                }
            }
        }

        window->visualization_overlay.set_new_overlay_instance(new_overlay_instance);
    }
} // namespace FluidUi