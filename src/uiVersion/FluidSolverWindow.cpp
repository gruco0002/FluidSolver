#include "FluidSolverWindow.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <core/timestep/DynamicCFLTimestep.hpp>
#include <core/timestep/ConstantTimestep.hpp>
#include "ImguiHelper.hpp"
#include <core/visualizer/GLParticleRenderer.hpp>
#include <thread>
#include <chrono>
#include <core/sensors/ParticleStatistics.hpp>
#include <Paths.hpp>
#include <core/Log.hpp>

FluidUi::FluidSolverWindow::FluidSolverWindow(const std::string& title, int width, int height) : sim_worker_thread(
	&FluidSolverWindow::sim_worker_thread_main, this), Window(title, width,
		height) {

}

void FluidUi::FluidSolverWindow::load() {
	// set the current fluid solver type
	current_type = &solver_types.types[0];
	create_empty_simulation();

	// initialize imgui and window
	ImGuiHelper::Init(this->GetWindowHandler());
	OnKeyPressed.Subscribe([=](int key) {
		if (key == GLFW_KEY_SPACE) {
			running = !running;
		}
		});

	// setup the windows
	setup_windows();
}

void FluidUi::FluidSolverWindow::unload() {
	sim_worker_thread_should_terminate = true;
	sim_worker_thread.join();
	ImGuiHelper::Uninit();
}

void FluidUi::FluidSolverWindow::render() {


	if (!sim_worker_thread_working && !sim_worker_thread_done) {
		if (running) {
			if (asynchronous_simulation) {
				sim_worker_thread_working = true;
			}
			else {
				simulation.execute_simulation_step();
			}
		}
	}

	if (!sim_worker_thread_working && sim_worker_thread_done) {
		sim_worker_thread_done = false;
	}

	if (!sim_worker_thread_working) {
		simulation.visualize();
		glFlush();
	}

	// render to screen
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, GetFramebufferWidth(), GetFramebufferHeight());

	ImGuiHelper::PreRender();

	// creating the dockspace
	auto id = ImGui::DockSpaceOverViewport();

	render_visualization_window();
	uiLayer.render();

	ImGuiHelper::PostRender();

}

void FluidUi::FluidSolverWindow::create_empty_simulation()
{
	simulation = FluidSolver::Simulation();

	simulation.parameters.collection = new FluidSolver::ParticleCollection();
	simulation.parameters.collection->add_type<FluidSolver::MovementData>();
	simulation.parameters.collection->add_type<FluidSolver::ParticleData>();
	simulation.parameters.collection->add_type<FluidSolver::ParticleInfo>();
	simulation.parameters.collection->add_type<FluidSolver::ExternalForces>();
	simulation.parameters.rest_density = 1.0f;
	simulation.parameters.particle_size = 1.0f;
	simulation.parameters.gravity = 9.81f;

	simulation.parameters.fluid_solver = current_type->create_type();
	simulation.parameters.timestep = new FluidSolver::ConstantTimestep();
	simulation.parameters.visualizer = new FluidSolver::GLParticleRenderer();

	simulation.parameters.sensor_storage = new FluidSolver::SensorDataStorage();
	simulation.parameters.sensors.push_back(new FluidSolver::ParticleStatisticsSensor());

	simulation.parameters.invalidate = true;

	FluidSolver::Log::message("Loaded empty scenario");

	on_new_simulation();
}

void FluidUi::FluidSolverWindow::on_new_simulation()
{
	this->current_type = this->solver_types.query_type(simulation.parameters.fluid_solver);
}

void FluidUi::FluidSolverWindow::render_visualization_window() {
	ImGui::Begin("Simulation Visualization");

	auto glRenderer = dynamic_cast<FluidSolver::GLRenderer*>(simulation.parameters.visualizer);
	if (glRenderer == nullptr) {
		ImGui::Text("No OpenGL compatible visualizer!");
	}
	else {
		auto tex = glRenderer->get_render_target();
		// render visualization
		auto maxRegion = ImGui::GetContentRegionMax();
		maxRegion.x -= 20.0f;
		maxRegion.y -= 30.0f;
		float width = 0.0f;
		float height = 0.0f;
		if ((float)tex->getWidth() / (float)tex->getHeight() * maxRegion.y > maxRegion.x) {
			// height is too small
			width = maxRegion.x;
			height = (float)tex->getHeight() / (float)tex->getWidth() * maxRegion.x;
		}
		else {
			height = maxRegion.y;
			width = (float)tex->getWidth() / (float)tex->getHeight() * maxRegion.y;
		}


		// im gui image location finder trick
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		if (ImGui::BeginChild("SimView")) {

			bool imageHasFocus = ImGui::IsWindowHovered();
			auto pos = ImGui::GetWindowPos();
			auto padding = ImGui::GetStyle().WindowPadding;
			auto windowPosition = glm::vec2(pos.x + padding.x, pos.y + padding.y);
			auto windowSize = glm::vec2(width, height);

			ImGui::PushItemWidth(width);

			ImGui::Image((void*)tex->GetID(), ImVec2(width, height));

			ImGui::PopItemWidth();

		}
		ImGui::EndChild();

		ImGui::PopStyleVar(3);

	}
	ImGui::End();
}

void FluidUi::FluidSolverWindow::setup_windows() {
	uiLayer.window = this;
	uiLayer.initialize();
}

void FluidUi::FluidSolverWindow::sim_worker_thread_main() {
	using namespace std::chrono_literals;
	while (!sim_worker_thread_should_terminate) {
		if (!running || !asynchronous_simulation) {
			std::this_thread::sleep_for(100ms);
		}
		else {
			if (sim_worker_thread_working) {
				simulation.execute_simulation_step();
				sim_worker_thread_done = true;
				sim_worker_thread_working = false;
			}
		}
	}
}

bool FluidUi::FluidSolverWindow::is_done_working() const {
	return !sim_worker_thread_working;
}
