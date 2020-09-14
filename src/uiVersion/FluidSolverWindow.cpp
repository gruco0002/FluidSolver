#include "FluidSolverWindow.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <core/timestep/DynamicCFLTimestep.hpp>
#include <core/timestep/ConstantTimestep.hpp>
#include "ImguiHelper.hpp"
#include <core/fluidSolver/IISPHFluidSolver.hpp>
#include <core/fluidSolver/SESPHFluidSolver.hpp>

FluidSolverWindow::FluidSolverWindow(const std::string &title, int width, int height) : Window(title, width, height) {

}

void FluidSolverWindow::load() {
    ImGuiHelper::Init(this->GetWindowHandler());
    set_default_simulation_parameters();
    set_visualizer_parameters();
    load_scenario("../scenarios/boundaryTest.chai");
    OnKeyPressed.Subscribe([=](int key) {
        if (key == GLFW_KEY_SPACE) {
            running = !running;
        }
    });
}

void FluidSolverWindow::unload() {
    delete scenario;
    ImGuiHelper::Uninit();
}

void FluidSolverWindow::render() {

    if (running) {
        simulation.execute_simulation_step();
    }

    simulation.visualize();
    glFlush();

    // render to screen
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, GetFramebufferWidth(), GetFramebufferHeight());

    ImGuiHelper::PreRender();

    render_visualization_window();

    ImGuiHelper::PostRender();

}

void FluidSolverWindow::load_scenario(const std::string &filepath) {
    delete scenario;
    scenario = nullptr;
    scenario = new FluidSolver::Scenario(filepath);
    FLUID_ASSERT(scenario != nullptr)
    simulation.parameters.collection = nullptr;

    simulation.parameters.collection = &scenario->data.collection;
    simulation.parameters.rest_density = scenario->data.rest_density;
    simulation.parameters.particle_size = scenario->data.particle_size;

    std::cout << "Loaded " << scenario->data.name << std::endl;
}

void FluidSolverWindow::set_default_simulation_parameters() {
    simulation.parameters.fluid_solver = new FluidSolver::IISPHFluidSolver();
    simulation.parameters.timestep = new FluidSolver::ConstantTimestep(0.001f);
    simulation.parameters.gravity = 9.81f;
    simulation.parameters.visualizer = new ParticleRenderer();
}

void FluidSolverWindow::render_visualization_window() {
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

void FluidSolverWindow::set_visualizer_parameters() {
    FLUID_ASSERT(simulation.parameters.visualizer != nullptr)

    simulation.parameters.visualizer->setSimulationViewArea({-15, 15, 15, -15});
}
