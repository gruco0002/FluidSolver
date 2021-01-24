#ifndef FLUIDSOLVER_FLUIDSOLVERWINDOW_HPP
#define FLUIDSOLVER_FLUIDSOLVERWINDOW_HPP

#include <engine/Window.hpp>
#include <engine/RectangleRenderer.hpp>
#include "FluidSolverTypes.hpp"
#include <engine/graphics/Framebuffer.hpp>
#include <core/Simulation.hpp>
#include <uiVersion/userInterface/UiLayer.hpp>
#include <thread>

namespace FluidUi {
	class FluidSolverWindow : public Engine::Window {

	public:
		explicit FluidSolverWindow(const std::string& title, int width = 800, int height = 600);

	protected:
		void unload() override;

		void render() override;

		void load() override;

	public:


		FluidSolver::Simulation simulation;



		bool running = false;

		bool asynchronous_simulation = true;

		bool is_done_working() const;

		bool is_safe_to_access_simulation_data() const;

		const FluidSolverTypes solver_types;

		const FluidSolverTypes::FluidSolverType* current_type = nullptr;

		void create_empty_simulation();

		void on_new_simulation();

	private:

		enum class SimWorkerThreadStatus {
			SimWorkerThreadStatusWork,
			SimWorkerThreadStatusDone,
			SimWorkerThreadStatusWaitForWork,
		} sim_worker_status = SimWorkerThreadStatus::SimWorkerThreadStatusWaitForWork;
		void sim_worker_thread_main();
		std::thread sim_worker_thread;
		bool sim_worker_thread_should_terminate = false;


		void render_visualization_window();

		void setup_windows();

		void execute_one_simulation_step();
		bool simulation_changed_compared_to_visualization = false;


		UiLayer uiLayer;


		FluidSolver::Image render_image_copy;
		bool render_image_updated = false;
		Engine::Graphics::Texture2D* rendered_image = nullptr;
		void visualize_simulation(bool called_from_worker_thread = false);

		bool simulation_visualization_window_in_foreground = false;


	};
}

#endif //FLUIDSOLVER_FLUIDSOLVERWINDOW_HPP
