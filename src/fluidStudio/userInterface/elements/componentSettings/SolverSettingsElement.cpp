#include "SolverSettingsElement.hpp"

#include "ImguiHelper.hpp"
#include "fluidSolver/IISPHFluidSolver.hpp"
#include "fluidSolver/IISPHFluidSolver3D.hpp"
#include "fluidSolver/SESPHFluidSolver.hpp"
#include "fluidSolver/SESPHFluidSolver3D.hpp"
#include "userInterface/StyledImGuiElements.hpp"

namespace FluidStudio {

    void SolverSettingsElement::update() {
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

    void SolverSettingsElement::update_solver_parameters() {
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
} // namespace FluidStudio