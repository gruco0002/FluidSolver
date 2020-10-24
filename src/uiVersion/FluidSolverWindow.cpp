#include "FluidSolverWindow.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <core/timestep/DynamicCFLTimestep.hpp>
#include <core/timestep/ConstantTimestep.hpp>
#include "ImguiHelper.hpp"

#include <thread>
#include <chrono>

FluidUi::FluidSolverWindow::FluidSolverWindow(const std::string &title, int width, int height) : sim_worker_thread(
        &FluidSolverWindow::sim_worker_thread_main, this), Window(title, width,
                                                                  height) {

}

void FluidUi::FluidSolverWindow::load() {
    current_type = &solver_types.types[0];

    ImGuiHelper::Init(this->GetWindowHandler());
    set_default_simulation_parameters();
    load_scenario("../scenarios/boundaryTest.chai");
    OnKeyPressed.Subscribe([=](int key) {
        if (key == GLFW_KEY_SPACE) {
            running = !running;
        }
    });

    setup_windows();
}

void FluidUi::FluidSolverWindow::unload() {
    sim_worker_thread_should_terminate = true;
    sim_worker_thread.join();
    delete scenario;
    ImGuiHelper::Uninit();
}

void FluidUi::FluidSolverWindow::render() {


    if (!sim_worker_thread_working && !sim_worker_thread_done) {
        if (running) {
            if (asynchronous_simulation) {
                sim_worker_thread_working = true;
            } else {
                simulation.execute_simulation_step();
            }
        }
    }

    if(!sim_worker_thread_working && sim_worker_thread_done){
        sim_worker_thread_done = false;
    }

    simulation.visualize(!sim_worker_thread_working);
    glFlush();

    // render to screen
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, GetFramebufferWidth(), GetFramebufferHeight());

    ImGuiHelper::PreRender();

    render_visualization_window();
    uiLayer.render();

    ImGuiHelper::PostRender();

}

void FluidUi::FluidSolverWindow::load_scenario(const std::string &filepath) {
    delete scenario;
    scenario = nullptr;
    scenario = new FluidSolver::Scenario(filepath);
    FLUID_ASSERT(scenario != nullptr)
    simulation.parameters.collection = nullptr;

    simulation.parameters.collection = &scenario->data.collection;
    simulation.parameters.rest_density = scenario->data.rest_density;
    simulation.parameters.particle_size = scenario->data.particle_size;
    simulation.parameters.entities = scenario->data.entities;
    simulation.parameters.invalidate = true;

    set_visualizer_parameters();

    std::cout << "Loaded " << scenario->data.name << std::endl;
}

void FluidUi::FluidSolverWindow::set_default_simulation_parameters() {
    simulation.parameters.fluid_solver = current_type->create_type();
    simulation.parameters.timestep = new FluidSolver::ConstantTimestep(0.01f);
    simulation.parameters.visualizer = new ParticleRenderer();
    simulation.parameters.gravity = 9.81f;
    simulation.parameters.invalidate = true;


}

void FluidUi::FluidSolverWindow::render_visualization_window() {
    ImGui::Begin("Simulation Visualization");

    auto glRenderer = dynamic_cast<IOpenGLVisualizer *>(simulation.parameters.visualizer);
    if (glRenderer == nullptr) {
        ImGui::Text("No OpenGL compatible visualizer!");
    } else {
        auto tex = glRenderer->GetTexture();
        // render visualization
        auto maxRegion = ImGui::GetContentRegionMax();
        maxRegion.x -= 20.0f;
        maxRegion.y -= 30.0f;
        float width = 0.0f;
        float height = 0.0f;
        if ((float) tex->getWidth() / (float) tex->getHeight() * maxRegion.y > maxRegion.x) {
            // height is too small
            width = maxRegion.x;
            height = (float) tex->getHeight() / (float) tex->getWidth() * maxRegion.x;
        } else {
            height = maxRegion.y;
            width = (float) tex->getWidth() / (float) tex->getHeight() * maxRegion.y;
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

            ImGui::Image((void *) tex->GetID(), ImVec2(width, height));

            ImGui::PopItemWidth();

        }
        ImGui::EndChild();

        ImGui::PopStyleVar(3);

    }
    ImGui::End();
}

void FluidUi::FluidSolverWindow::set_visualizer_parameters() {
    FLUID_ASSERT(simulation.parameters.visualizer != nullptr)
    FLUID_ASSERT(scenario != nullptr);
    simulation.parameters.visualizer->setSimulationViewArea(
            {scenario->data.viewport.left, scenario->data.viewport.top, scenario->data.viewport.right,
             scenario->data.viewport.bottom});
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
        } else {
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
