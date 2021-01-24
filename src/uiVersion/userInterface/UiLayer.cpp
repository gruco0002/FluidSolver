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


void FluidUi::UiLayer::render_component_panel()
{
	if (ImGui::Begin("Components")) {

		// Draw components
		render_component_node("Solver", { Component::Kind::Solver, 0 });
		render_component_node("Visualizer", { Component::Kind::Visualizer, 0 });
		render_component_node("Timestep", { Component::Kind::Timestep, 0 });

		for (size_t i = 0; i < window->simulation.parameters.sensors.size(); i++) {
			auto sen = window->simulation.parameters.sensors[i];
			render_component_node(sen->parameters.name.c_str(), { Component::Kind::Sensor, i });
		}


		// handle mouse click outside
		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			selection = {};

		// right click outside
		bool is_safe = window->is_safe_to_access_simulation_data();
		if (ImGui::BeginPopupContextWindow(0, 1, false)) {
			if (ImGui::BeginMenu("Add Sensor"))
			{
				if (ImGui::MenuItem("Global Density", nullptr, nullptr, is_safe)) {
					auto sen = new FluidSolver::Sensors::GlobalDensitySensor();
					sen->parameters.name = "Sensor " + std::to_string(window->simulation.parameters.sensors.size() + 1);
					window->simulation.parameters.sensors.push_back(sen);
				}
				if (ImGui::MenuItem("Global Pressure", nullptr, nullptr, is_safe)) {
					auto sen = new FluidSolver::Sensors::GlobalPressureSensor();
					sen->parameters.name = "Sensor " + std::to_string(window->simulation.parameters.sensors.size() + 1);
					window->simulation.parameters.sensors.push_back(sen);
				}
				if (ImGui::MenuItem("Global Velocity", nullptr, nullptr, is_safe)) {
					auto sen = new FluidSolver::Sensors::GlobalVelocitySensor();
					sen->parameters.name = "Sensor " + std::to_string(window->simulation.parameters.sensors.size() + 1);
					window->simulation.parameters.sensors.push_back(sen);
				}
				if (ImGui::MenuItem("Global Energy", nullptr, nullptr, is_safe)) {
					auto sen = new FluidSolver::Sensors::GlobalEnergySensor();
					sen->parameters.name = "Sensor " + std::to_string(window->simulation.parameters.sensors.size() + 1);
					window->simulation.parameters.sensors.push_back(sen);
				}
				if (ImGui::MenuItem("Global Particle Count", nullptr, nullptr, is_safe)) {
					auto sen = new FluidSolver::Sensors::GlobalParticleCountSensor();
					sen->parameters.name = "Sensor " + std::to_string(window->simulation.parameters.sensors.size() + 1);
					window->simulation.parameters.sensors.push_back(sen);
				}
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Add Entity"))
			{
				// TODO: implement
			}
			ImGui::EndPopup();
		}
	}
	ImGui::End();

}

void FluidUi::UiLayer::render_simulation_controls()
{
	ImGui::Begin("Simulation Controls");
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
	ImGui::End();
}

void FluidUi::UiLayer::render_component_node(const char* name, const Component& component)
{
	ImGuiTreeNodeFlags flags = ((component == selection) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow
		| ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;

	bool opened = ImGui::TreeNodeEx((void*)((size_t)name + component.index * 1565412), flags, name);

	if (ImGui::IsItemClicked())
	{
		selection = component;
	}


	bool component_deleted = false;

	if (component.can_delete()) {
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Component"))
				component_deleted = true;

			ImGui::EndPopup();
		}
	}

	if (opened)
	{
		ImGui::TreePop();
	}

	if (component_deleted)
	{
		// reset selction if required
		if (selection == component)
			selection = {};

		// remove component
		delete_component(component);
	}
}

void FluidUi::UiLayer::render_component_settings(const Component& component)
{
	if (component.kind == Component::Kind::Solver) {
		render_solver_component();
	}
	else if (component.kind == Component::Kind::Timestep) {
		render_timestep_component();
	}
	else if (component.kind == Component::Kind::Sensor) {
		render_sensor_component(component.index);
	}
}

void FluidUi::UiLayer::render_solver_component()
{
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
}

void FluidUi::UiLayer::render_timestep_component()
{
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

}

const char* get_sensor_type_name(FluidSolver::ISensor* sen) {

	if (dynamic_cast<FluidSolver::Sensors::GlobalDensitySensor*>(sen)) {
		return "Global Density";
	}
	else if (dynamic_cast<FluidSolver::Sensors::GlobalPressureSensor*>(sen)) {
		return "Global Pressure";
	}
	else if (dynamic_cast<FluidSolver::Sensors::GlobalVelocitySensor*>(sen)) {
		return "Global Velocity";
	}
	else if (dynamic_cast<FluidSolver::Sensors::GlobalEnergySensor*>(sen)) {
		return "Global Energy";
	}
	else if (dynamic_cast<FluidSolver::Sensors::GlobalParticleCountSensor*>(sen)) {
		return "Global Particle Count";
	}
	return "UNKNOWN";
}

void FluidUi::UiLayer::render_sensor_component(size_t index)
{
	FLUID_ASSERT(window != nullptr);
	FLUID_ASSERT(index < window->simulation.parameters.sensors.size());
	auto sen = window->simulation.parameters.sensors[index];
	BeginSubsection("Sensor", [&]() {
		ImGui::LabelText("Type", get_sensor_type_name(sen));
		if (ImGui::Button("Open Graph")) {
			statisticsUi.open_sensor_window(index);
		}
		ImGui::InputText("Name", &sen->parameters.name);
		ImGui::Checkbox("Save to File", &sen->parameters.save_to_file);
		});

	if (dynamic_cast<FluidSolver::Sensors::GlobalEnergySensor*>(sen)) {
		render_global_energy_sensor_component(dynamic_cast<FluidSolver::Sensors::GlobalEnergySensor*>(sen));
	}

}

void FluidUi::UiLayer::render_global_energy_sensor_component(FluidSolver::Sensors::GlobalEnergySensor* sen)
{
	BeginSubsection("Zero-Levels", [&]() {
		ImGui::InputFloat("Zero Height", &sen->settings.relative_zero_height);
		});
}

void FluidUi::UiLayer::delete_component(const Component& component)
{
	if (component.kind == Component::Kind::Sensor) {
		// delete the sensor if existing

		// check for existance
		if (window->simulation.parameters.sensors.size() <= component.index) {
			return;
		}

		// delete the sensor
		for (size_t i = component.index; i < window->simulation.parameters.sensors.size() - 1; i++) {
			window->simulation.parameters.sensors[i] = window->simulation.parameters.sensors[i + 1];
		}
		window->simulation.parameters.sensors.pop_back();

	}
}

void FluidUi::UiLayer::render() {

	render_menu();
	render_simulation_controls();

	// render component controls	
	render_component_panel();
	render_component_properties_panel();

	// render other windows
	statisticsUi.render();
	logWindow.render();
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
						window->simulation = simulation;
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

void FluidUi::UiLayer::render_component_properties_panel()
{
	if (ImGui::Begin("Properties")) {

		if (selection.kind != Component::Kind::None) {
			render_component_settings(selection);
		}

	}
	ImGui::End();
}

bool FluidUi::UiLayer::Component::operator==(const Component& other) const
{
	return kind == other.kind && index == other.index;
}

bool FluidUi::UiLayer::Component::operator!=(const Component& other) const
{
	return !(*this == other);
}

bool FluidUi::UiLayer::Component::can_delete() const
{
	switch (kind) {
	case Kind::Sensor:
		return true;

	}

	return false;
}
