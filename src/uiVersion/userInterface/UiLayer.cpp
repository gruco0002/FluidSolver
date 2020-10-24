#include "UiLayer.hpp"
#include "uiVersion/ImguiHelper.hpp"
#include <string>
#include <functional>
#include "uiVersion/FluidSolverWindow.hpp"

#include "core/fluidSolver/SESPHFluidSolver.hpp"
#include "core/fluidSolver/IISPHFluidSolver.hpp"


static void BeginSubsection(const std::string &name, const std::function<void()> &fnc) {
    const ImGuiTreeNodeFlags treeNodeFlags =
            ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth |
            ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 4});
    ImGui::Separator();
    bool open = ImGui::TreeNodeEx((void *) name.c_str(), treeNodeFlags, name.c_str());
    ImGui::PopStyleVar();

    if (open) {
        fnc();
        ImGui::TreePop();
    }


}

void FluidUi::UiLayer::render() {

    ImGui::Begin("Properties");

    BeginSubsection("Simulation", [=]() {

        if (window->running)
            ImGui::TextColored(ImColor(0.1f, 0.8f, 0.1f), "Running");
        else
            ImGui::TextColored(ImColor(0.8f, 0.1f, 0.1f), "Paused");

        if (ImGui::Button("Run")) {
            this->window->running = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Pause")) {
            this->window->running = false;
        }

        ImGui::Checkbox("Asynchronous", &window->asynchronous_simulation);
    });

    BeginSubsection("Scenarios", [=]() {
        scenarios.render();
    });

    BeginSubsection("Solver Setup", [=]() {

        bool can_change = !window->asynchronous_simulation || (!window->running && window->is_done_working());

        if (ImGui::BeginCombo("Solver", window->current_type->name_solver.c_str())) {
            for (auto &name : window->solver_types.names_solver) {
                if (ImGui::Selectable(name.c_str(), window->current_type->name_solver == name)) {
                    FluidSolverTypes::FluidSolverTypeQuery q;
                    q.name_solver = name;
                    q.name_neighborhood_search = window->current_type->name_neighborhood_search;
                    q.name_kernel = window->current_type->name_kernel;
                    auto t = window->solver_types.query_type(q);
                    if (t && can_change) {
                        window->current_type = t;

                        // set the new type
                        delete window->simulation.parameters.fluid_solver;
                        window->simulation.parameters.fluid_solver = t->create_type();
                    }
                }
            }
            ImGui::EndCombo();
        }

        if (ImGui::BeginCombo("N. Search", window->current_type->name_neighborhood_search.c_str())) {
            for (auto &name : window->solver_types.names_neighborhood_search) {
                if (ImGui::Selectable(name.c_str(), window->current_type->name_neighborhood_search == name)) {
                    FluidSolverTypes::FluidSolverTypeQuery q;
                    q.name_solver = window->current_type->name_solver;
                    q.name_neighborhood_search = name;
                    q.name_kernel = window->current_type->name_kernel;
                    auto t = window->solver_types.query_type(q);
                    if (t && can_change) {
                        window->current_type = t;

                        // set the new type
                        delete window->simulation.parameters.fluid_solver;
                        window->simulation.parameters.fluid_solver = t->create_type();
                    }
                }
            }
            ImGui::EndCombo();
        }

        if (ImGui::BeginCombo("Kernel", window->current_type->name_kernel.c_str())) {
            for (auto &name : window->solver_types.names_kernel) {
                if (ImGui::Selectable(name.c_str(), window->current_type->name_kernel == name)) {
                    FluidSolverTypes::FluidSolverTypeQuery q;
                    q.name_solver = window->current_type->name_solver;
                    q.name_neighborhood_search = window->current_type->name_neighborhood_search;
                    q.name_kernel = name;
                    auto t = window->solver_types.query_type(q);
                    if (t && can_change) {
                        window->current_type = t;

                        // set the new type
                        delete window->simulation.parameters.fluid_solver;
                        window->simulation.parameters.fluid_solver = t->create_type();
                    }
                }
            }
            ImGui::EndCombo();
        }

        if (!can_change) {
            ImGui::TextColored(ImColor(0.8f, 0.1f, 0.1f), "Config change not possible");
            ImGui::TextWrapped(
                    "Since you are running the simulation in asynchronous mode, you cannot change the solver setup if the simulation is running or has not stopped running!");
        }

    });

    BeginSubsection("Timestep", [=]() {
        // TODO
        ImGui::Text("todo");
    });

    if (window->current_type->settings_type == FluidSolverTypes::SolverSettingsTypeSESPH) {
        BeginSubsection("SESPH", [=]() {
            auto v = (FluidSolver::SESPHSettings *) window->current_type->get_settings(
                    window->simulation.parameters.fluid_solver);

            ImGui::InputFloat("Viscosity", &v->Viscosity);
            ImGui::InputFloat("Stiffness", &v->StiffnessK);
        });
    }

    if (window->current_type->settings_type == FluidSolverTypes::SolverSettingsTypeIISPH) {
        BeginSubsection("IISPH", [=]() {
            auto v = (FluidSolver::IISPHSettings *) window->current_type->get_settings(
                    window->simulation.parameters.fluid_solver);

            ImGui::InputFloat("Viscosity", &v->Viscosity);
            ImGui::InputFloat("Max. Density Err.", &v->MaxDensityErrorAllowed);
            ImGui::InputInt("Min. Iterations", (int *) &v->MinNumberOfIterations);
            ImGui::InputInt("Max. Iterations", (int *) &v->MaxNumberOfIterations);
            ImGui::InputFloat("Gamma", &v->Gamma);
            ImGui::InputFloat("Omega", &v->Omega);
        });
    }

    ImGui::End();

}

void FluidUi::UiLayer::initialize() {
    scenarios.window = window;
    scenarios.initialize();
}
