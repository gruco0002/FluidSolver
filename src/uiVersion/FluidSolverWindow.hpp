#ifndef FLUIDSOLVER_FLUIDSOLVERWINDOW_HPP
#define FLUIDSOLVER_FLUIDSOLVERWINDOW_HPP

#include <engine/Window.hpp>
#include <engine/RectangleRenderer.hpp>
#include "ParticleRenderer.hpp"
#include "FluidSolverTypes.hpp"
#include <engine/graphics/Framebuffer.hpp>
#include <core/Simulation.hpp>
#include <core/scenario/Scenario.hpp>
#include <uiVersion/userInterface/ScenariosWindow.hpp>
#include <uiVersion/userInterface/UiLayer.hpp>

namespace FluidUi {
    class FluidSolverWindow : public Engine::Window {

    public:
        explicit FluidSolverWindow(const std::string &title, int width = 800, int height = 600);

    protected:
        void unload() override;

        void render() override;

        void load() override;

    public:

        FluidSolver::Scenario *scenario = nullptr;

        FluidSolver::Simulation simulation;

        void load_scenario(const std::string &filepath);

        bool running = false;

        bool asynchronous_simulation = true;

        bool is_done_working() const;

        const FluidSolverTypes solver_types;

        const FluidSolverTypes::FluidSolverType* current_type = nullptr;

    private:

        void render_visualization_window();

        void set_visualizer_parameters();

        void set_default_simulation_parameters();

    private:

        void setup_windows();


        UiLayer uiLayer;

        void sim_worker_thread_main();
        std::thread sim_worker_thread;
        bool sim_worker_thread_working = false;
        bool sim_worker_thread_done = false;
        bool sim_worker_thread_should_terminate = false;

    };
}

#endif //FLUIDSOLVER_FLUIDSOLVERWINDOW_HPP
