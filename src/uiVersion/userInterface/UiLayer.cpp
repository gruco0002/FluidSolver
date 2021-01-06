#include "UiLayer.hpp"
#include "uiVersion/ImguiHelper.hpp"
#include <string>
#include <functional>
#include "uiVersion/FluidSolverWindow.hpp"

#include "core/fluidSolver/SESPHFluidSolver.hpp"
#include "core/fluidSolver/IISPHFluidSolver.hpp"

#include "core/timestep/ConstantTimestep.hpp"
#include "core/timestep/DynamicCFLTimestep.hpp"

#include <nfd.h>
#include "core/scenario/SimulationSerializer.hpp"

static void BeginSubsection(const std::string& name, const std::function<void()>& fnc) {
	const ImGuiTreeNodeFlags treeNodeFlags =
		ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth |
		ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
	ImGui::Separator();
	bool open = ImGui::TreeNodeEx((void*)name.c_str(), treeNodeFlags, name.c_str());
	ImGui::PopStyleVar();

	if (open) {
		fnc();
		ImGui::TreePop();
	}


}

void FluidUi::UiLayer::render() {

	render_menu();

	statisticsUi.render();
	logWindow.render();


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


	BeginSubsection("Solver Setup", [=]() {

		bool can_change = !window->asynchronous_simulation || (!window->running && window->is_done_working());

		if (ImGui::BeginCombo("Solver", window->current_type->name_solver.c_str())) {
			for (auto& name : window->solver_types.names_solver) {
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
			for (auto& name : window->solver_types.names_neighborhood_search) {
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
			for (auto& name : window->solver_types.names_kernel) {
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
		auto ct = dynamic_cast<FluidSolver::ConstantTimestep*>(window->simulation.parameters.timestep);
		auto dt = dynamic_cast<FluidSolver::DynamicCFLTimestep*>(window->simulation.parameters.timestep);

		if (ImGui::BeginCombo("Type", ct ? "Constant" : "Dynamic CFL")) {
			if (ImGui::Selectable("Constant", ct != nullptr)) {
				if (ct == nullptr) {
					ct = nullptr;
					dt = nullptr;
					delete window->simulation.parameters.timestep;

					ct = new FluidSolver::ConstantTimestep();
					window->simulation.parameters.timestep = ct;
					window->simulation.parameters.invalidate = true;
				}
			}
			if (ImGui::Selectable("Dynamic CFL", dt != nullptr)) {
				if (dt == nullptr) {
					ct = nullptr;
					dt = nullptr;

					delete window->simulation.parameters.timestep;
					dt = new FluidSolver::DynamicCFLTimestep();
					window->simulation.parameters.timestep = dt;
					window->simulation.parameters.invalidate = true;
				}
			}
			ImGui::EndCombo();
		}

		if (ct) {
			if (ImGui::InputFloat("Timestep", &ct->settings.timestep, 0.0f, 0.0f, "%.5f")) {
				ct->settings.timestep = std::max(ct->settings.timestep, 0.00001f);
			}
			if (ImGui::Button("Reset")) {
				ct->settings = FluidSolver::ConstantTimestep::ConstantTimestepSettings();
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
				dt->settings = FluidSolver::DynamicCFLTimestep::DynamicCFLTimestepSettings();
			}
		}
		});

	if (window->current_type->settings_type == FluidSolverTypes::SolverSettingsTypeSESPH) {
		BeginSubsection("SESPH", [=]() {
			auto v = (FluidSolver::SESPHSettings*)window->current_type->get_settings(
				window->simulation.parameters.fluid_solver);
			render_solver_parameters();
			ImGui::Separator();
			ImGui::InputFloat("Viscosity", &v->Viscosity);
			ImGui::InputFloat("Stiffness", &v->StiffnessK);
			});
	}

	if (window->current_type->settings_type == FluidSolverTypes::SolverSettingsTypeIISPH) {
		BeginSubsection("IISPH", [=]() {

			render_solver_parameters();
			ImGui::Separator();

			auto v = (FluidSolver::IISPHSettings*)window->current_type->get_settings(
				window->simulation.parameters.fluid_solver);

			ImGui::InputFloat("Viscosity", &v->Viscosity);
			ImGui::InputFloat("Max. Density Err.", &v->MaxDensityErrorAllowed);
			ImGui::InputInt("Min. Iterations", (int*)&v->MinNumberOfIterations);
			ImGui::InputInt("Max. Iterations", (int*)&v->MaxNumberOfIterations);
			ImGui::InputFloat("Gamma", &v->Gamma);
			ImGui::InputFloat("Omega", &v->Omega);
			});
	}

	ImGui::End();

}

void FluidUi::UiLayer::initialize() {
	statisticsUi.window = window;
	statisticsUi.initialize();
	logWindow.window = window;
	logWindow.initialize();
}

void FluidUi::UiLayer::render_solver_parameters() {
	ImGui::InputFloat("Gravity", &window->simulation.parameters.fluid_solver->parameters.gravity);
	ImGui::InputFloat("Rest Density", &window->simulation.parameters.fluid_solver->parameters.rest_density);
	ImGui::InputFloat("Particle Size", &window->simulation.parameters.fluid_solver->parameters.particle_size);
}

void FluidUi::UiLayer::render_menu()
{

	static bool save_menu_open = false;



	if (ImGui::BeginMainMenuBar()) {

		if (ImGui::BeginMenu("File")) {

			bool can_change = !window->asynchronous_simulation || (!window->running && window->is_done_working());

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
					FluidSolver::SimulationSerializer s(path);
					auto simulation = s.load_from_file();
					if (!s.has_errors()) {
						window->simulation = s.load_from_file();
						window->on_new_simulation();
					}

				}
			}

			if (ImGui::MenuItem("Save", nullptr, false, can_change)) {
				save_menu_open = true;
			}

			ImGui::EndMenu();
		}


		ImGui::EndMainMenuBar();
	}


	if (ImGui::BeginPopupModal("Save Simulation")) {

		static char* path = nullptr;
		static char* particle_filepath = new char[1024]{ "particles.data" };
		static bool save_particle_data = true;


		ImGui::TextWrapped("Save the current simulation as yaml file. Optionally you can save the particle data. If it already exists and you do not want to override it, make sure to uncheck the checkbox and provide the name of the current particle data file relative to the yaml file in the text field.");

		ImGui::Separator();

		if (ImGui::Button("Choose")) {
			char* p = nullptr;

			auto res = NFD_SaveDialog("yaml", nullptr, &p);
			if (res == NFD_OKAY) {
				if (path != nullptr) free(path);
				path = p;
			}
			else {
				free(p);
			}

		}
		ImGui::SameLine();
		if (path != nullptr) {
			ImGui::LabelText("File", path);
		}
		else {
			ImGui::LabelText("File", "Not selected");
		}

		ImGui::Separator();
		ImGui::Checkbox("Save Particle Data", &save_particle_data);
		ImGui::InputText("Particle File", particle_filepath, 1024);

		if (ImGui::Button("Close")) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Save") && path != nullptr) {
			// Save
			FluidSolver::SimulationSerializer s(path, { save_particle_data, particle_filepath });
			s.save_to_file(window->simulation);
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
	if (save_menu_open) {
		save_menu_open = false;
		ImGui::OpenPopup("Save Simulation");
	}



}
