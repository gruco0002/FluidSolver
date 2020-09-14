#ifndef FLUIDSOLVER_FLUIDSOLVERWINDOW_HPP
#define FLUIDSOLVER_FLUIDSOLVERWINDOW_HPP

#include <engine/Window.hpp>
#include <engine/RectangleRenderer.hpp>
#include "ParticleRenderer.hpp"
#include <engine/graphics/Framebuffer.hpp>
#include <core/Simulation.hpp>
#include <core/scenario/Scenario.hpp>
#include <uiVersion/userInterface/MainUi.hpp>



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

private:

    void render_visualization_window();

    void set_visualizer_parameters();

    void set_default_simulation_parameters();
};


#endif //FLUIDSOLVER_FLUIDSOLVERWINDOW_HPP
