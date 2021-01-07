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

		const FluidSolverTypes solver_types;

		const FluidSolverTypes::FluidSolverType* current_type = nullptr;

		void create_empty_simulation();

		void on_new_simulation();

	private:

		void render_visualization_window();

		void setup_windows();


		UiLayer uiLayer;

		void sim_worker_thread_main();
		std::thread sim_worker_thread;
		bool sim_worker_thread_working = false;
		bool sim_worker_thread_done = false;
		bool sim_worker_thread_should_terminate = false;

		bool render_image_updated = false;
		bool simulation_changed = false;
		Engine::Graphics::Texture2D* rendered_image = nullptr;

	};
}

#endif //FLUIDSOLVER_FLUIDSOLVERWINDOW_HPP
