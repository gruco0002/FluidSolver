#include <glad/glad.h>

#include "FluidSolverWindow.hpp"

#include "ImguiHelper.hpp"
#include "Log.hpp"
#include "time/ConstantTimestepGenerator.hpp"
#include "visualizer/ContinousVisualizer.hpp"
#include "visualizer/GLParticleRenderer.hpp"
#include "visualizer/GLParticleRenderer3D.hpp"
#include "visualizer/GLRenderer.hpp"

#include <GLFW/glfw3.h>
#include <chrono>
#include <thread>

FluidUi::FluidSolverWindow::FluidSolverWindow(const std::string& title, int width, int height)
    : sim_worker_thread(&FluidSolverWindow::sim_worker_thread_main, this), Window(title, width, height),
      render_image_copy(0, 0) {
}

void FluidUi::FluidSolverWindow::load() {
    // set the current fluid solver type
    current_type = &solver_types.types[0];
    create_empty_simulation();

    // initialize imgui and window
    ImGuiHelper::Init(this->get_window_handler());

    // setup the windows
    setup_windows();

    set_width((int)(get_screen_width() * 0.9f));
    set_height((int)(get_screen_height() * 0.9f));
}

void FluidUi::FluidSolverWindow::unload() {
    sim_worker_thread_should_terminate = true;
    sim_worker_thread.join();
    ImGuiHelper::Uninit();

    delete rendered_image;
    rendered_image = nullptr;
}

void FluidUi::FluidSolverWindow::render() {
    if (sim_worker_status == SimWorkerThreadStatus::SimWorkerThreadStatusWaitForWork) {
        // prevent synchronous and asynchronous steps from happening while the worker thread is working or done working
        if (running) {
            if (asynchronous_simulation) {
                // async simulation -> invoke the worker thread to execute on step of simulation
                sim_worker_status = SimWorkerThreadStatus::SimWorkerThreadStatusWork;
            } else {
                // synced simulation (with the main thread) -> directly execute a simulation step
                execute_one_simulation_step();
            }
        }
    } else if (sim_worker_status == SimWorkerThreadStatus::SimWorkerThreadStatusDone) {
        // the worker thread has calculated one step of the simulation and is done with its work

        // finally let the worker thread accept new work in the next step
        sim_worker_status = SimWorkerThreadStatus::SimWorkerThreadStatusWaitForWork;
    }

    // visualize the simulation if required (the method will only do work if required)
    visualize_simulation();


    // render to screen
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, get_framebuffer_width(), get_framebuffer_height());

    ImGuiHelper::PreRender();

    // creating the dockspace
    auto id = ImGui::DockSpaceOverViewport();

    render_visualization_window();
    uiLayer.render();

    ImGuiHelper::PostRender();

    // update the camera
    update_camera();
}

void FluidUi::FluidSolverWindow::create_empty_simulation() {
    simulation.simulation = std::make_shared<FluidSolver::Simulator>();

    simulation.simulation->parameters.collection = std::make_shared<FluidSolver::ParticleCollection>();
    simulation.simulation->parameters.collection->add_type<FluidSolver::MovementData>();
    simulation.simulation->parameters.collection->add_type<FluidSolver::ParticleData>();
    simulation.simulation->parameters.collection->add_type<FluidSolver::ParticleInfo>();
    simulation.simulation->parameters.collection->add_type<FluidSolver::ExternalForces>();
    simulation.simulation->parameters.rest_density = 1.0f;
    simulation.simulation->parameters.particle_size = 1.0f;
    simulation.simulation->parameters.gravity = 9.81f;

    simulation.simulation->parameters.fluid_solver = current_type->create_type();
    simulation.simulation->parameters.timestep = std::make_shared<FluidSolver::ConstantTimestepGenerator>();
    simulation.visualizer = std::make_shared<FluidSolver::GLParticleRenderer>();

    // simulation.parameters.sensors.push_back(new FluidSolver::ParticleStatisticsSensor());

    simulation.simulation->parameters.invalidate = true;


    FluidSolver::Log::message("Loaded empty scenario");

    on_new_simulation();
}

void FluidUi::FluidSolverWindow::create_empty_3d_simulation() {
    simulation.simulation = std::make_shared<FluidSolver::Simulator>();

    simulation.simulation->parameters.collection = std::make_shared<FluidSolver::ParticleCollection>();
    simulation.simulation->parameters.collection->add_type<FluidSolver::MovementData3D>();
    simulation.simulation->parameters.collection->add_type<FluidSolver::ParticleData>();
    simulation.simulation->parameters.collection->add_type<FluidSolver::ParticleInfo>();
    simulation.simulation->parameters.collection->add_type<FluidSolver::ExternalForces3D>();
    simulation.simulation->parameters.rest_density = 1.0f;
    simulation.simulation->parameters.particle_size = 1.0f;
    simulation.simulation->parameters.gravity = 9.81f;

    simulation.simulation->parameters.fluid_solver = current_type->create_type();
    simulation.simulation->parameters.timestep = std::make_shared<FluidSolver::ConstantTimestepGenerator>();
    simulation.visualizer = std::make_shared<FluidSolver::GLParticleRenderer3D>();

    simulation.simulation->parameters.invalidate = true;


    FluidSolver::Log::message("Loaded empty 3d scenario");

    on_new_simulation();
}

void FluidUi::FluidSolverWindow::create_3d_test_simulation() {
    simulation.simulation = std::make_shared<FluidSolver::Simulator>();

    simulation.simulation->parameters.collection = std::make_shared<FluidSolver::ParticleCollection>();
    simulation.simulation->parameters.collection->add_type<FluidSolver::MovementData3D>();
    simulation.simulation->parameters.collection->add_type<FluidSolver::ParticleData>();
    simulation.simulation->parameters.collection->add_type<FluidSolver::ParticleInfo>();
    simulation.simulation->parameters.collection->add_type<FluidSolver::ExternalForces3D>();
    simulation.simulation->parameters.rest_density = 1.0f;
    simulation.simulation->parameters.particle_size = 1.0f;
    simulation.simulation->parameters.gravity = 9.81f;


    simulation.simulation->parameters.fluid_solver = current_type->create_type();
    simulation.simulation->parameters.timestep = std::make_shared<FluidSolver::ConstantTimestepGenerator>();
    simulation.visualizer = std::make_shared<FluidSolver::GLParticleRenderer3D>();

    // simulation.parameters.sensors.push_back(new FluidSolver::ParticleStatisticsSensor());

    auto sen = std::make_shared<FluidSolver::Sensors::SensorPlane>();
    sen->settings.origin = glm::vec3(0, -5, 5);
    sen->settings.span_x = glm::vec3(0.0f, 0.0f, 1.0f);
    sen->settings.span_y = glm::vec3(0.0f, 1.0f, 0.0f);
    sen->settings.width = 10;
    sen->settings.height = 10;
    sen->settings.number_of_samples_x = 100;
    sen->settings.number_of_samples_y = 100;
    sen->settings.min_image_value = 0.0f;
    sen->settings.max_image_value = 1.5f;
    simulation.simulation->parameters.sensors.push_back(sen);


    for (int x = -5; x < 5; x++) {
        for (int y = -5; y < 5; y++) {
            for (int z = -5; z < 5; z++) {
                auto index = simulation.simulation->parameters.collection->add();
                auto& pos = simulation.simulation->parameters.collection->get<FluidSolver::MovementData3D>(index);
                pos.position = glm::vec3((float)x, (float)y, (float)z);

                auto& info = simulation.simulation->parameters.collection->get<FluidSolver::ParticleInfo>(index);
                info.type = FluidSolver::ParticleType::ParticleTypeNormal;

                auto& data = simulation.simulation->parameters.collection->get<FluidSolver::ParticleData>(index);
                data.density = 1.0f;
                data.mass = 1.0f;
                data.pressure = 0.0f;
            }
        }
    }

    auto emit_boundary = [&](float x, float y, float z) {
        auto index = simulation.simulation->parameters.collection->add();
        auto& pos = simulation.simulation->parameters.collection->get<FluidSolver::MovementData3D>(index);
        pos.position = glm::vec3((float)x, (float)y, (float)z);

        auto& info = simulation.simulation->parameters.collection->get<FluidSolver::ParticleInfo>(index);
        info.type = FluidSolver::ParticleType::ParticleTypeBoundary;

        auto& data = simulation.simulation->parameters.collection->get<FluidSolver::ParticleData>(index);
        data.density = 1.0f;
        data.mass = 1.0f;
        data.pressure = 0.0f;
    };

    // ground
    for (int x = -8; x < 8; x++) {
        for (int y = -8; y < -5; y++) {
            for (int z = -8; z < 20; z++) {
                emit_boundary(x, y, z);
            }
        }
    }

    // -z wall
    for (int x = -8; x < 8; x++) {
        for (int y = -5; y < 10; y++) {
            for (int z = -8; z < -5; z++) {
                emit_boundary(x, y, z);
            }
        }
    }

    // z-wall
    for (int x = -8; x < 8; x++) {
        for (int y = -5; y < 10; y++) {
            for (int z = 17; z < 20; z++) {
                emit_boundary(x, y, z);
            }
        }
    }

    // -x wall
    for (int x = -8; x < -5; x++) {
        for (int y = -5; y < 10; y++) {
            for (int z = -5; z < 17; z++) {
                emit_boundary(x, y, z);
            }
        }
    }

    // x wall
    for (int x = 5; x < 8; x++) {
        for (int y = -5; y < 10; y++) {
            for (int z = -5; z < 17; z++) {
                emit_boundary(x, y, z);
            }
        }
    }


    FluidSolver::Log::message("Loaded 3d test data scenario");

    on_new_simulation();
}


void FluidUi::FluidSolverWindow::on_new_simulation() {
    this->current_type = this->solver_types.query_type(simulation.simulation->parameters.fluid_solver);
    simulation.simulation->manual_initialize();
    auto compatibility_report = simulation.simulation->check();
    if (compatibility_report.has_issues()) {
        compatibility_report.log_issues();
    }
    simulation_changed_compared_to_visualization = true;
    render_image_updated = true;
}

void FluidUi::FluidSolverWindow::visualizer_parameter_changed() {
    simulation_changed_compared_to_visualization = true;
}

void FluidUi::FluidSolverWindow::render_visualization_window() {
    bool in_foreground = false;

    ImGui::Begin("Simulation Visualization");

    in_foreground = in_foreground || ImGui::IsWindowFocused();

    auto glRenderer = std::dynamic_pointer_cast<FluidSolver::GLRenderer>(simulation.visualizer);
    if (simulation.visualizer == nullptr) {
        ImGui::Text("No visualizer is set!");
    } else {
        Engine::Graphics::Texture2D* tex = nullptr;
        if (glRenderer == nullptr) {
            // ImGui::Text("No OpenGL compatible visualizer!");

            // Use the slower render mechanism
            // copy the image data to the gpu and render it as a texture
            FluidSolver::ISimulationVisualizer::Size size;
            size.width = render_image_copy.width();
            size.height = render_image_copy.height();

            if (render_image_updated) {
                // create or recreate the gpu image
                if (rendered_image == nullptr || rendered_image->getWidth() != size.width ||
                        rendered_image->getHeight() != size.height) {
                    delete rendered_image;
                    rendered_image = nullptr;

                    // create new image
                    auto color_settings = new Engine::Graphics::Texture2DSettings();
                    color_settings->GenerateMipmaps = false;
                    rendered_image = new Engine::Graphics::Texture2D(size.width, size.height, color_settings, GL_RGBA,
                            Engine::ComponentType::ComponentTypeUnsignedByte);
                }

                // update the gpu image
                rendered_image->SetData(render_image_copy.data(), render_image_copy.size());

                render_image_updated = false;
            }

            tex = rendered_image;
        } else {
            // get the texture from the opengl renderer
            tex = glRenderer->get_render_target();

            // delete any existing image
            delete rendered_image;
            rendered_image = nullptr;
        }

        if (tex == nullptr) {
            ImGui::Text("No OpenGL compatible visualizer!");
        } else {
            // render visualization
            auto maxRegion = ImGui::GetContentRegionMax();
            maxRegion.x -= 20.0f;
            maxRegion.y -= 30.0f;
            visualizer_window_size.width = maxRegion.x;
            visualizer_window_size.height = maxRegion.y;
            float width = 0.0f;
            float height = 0.0f;
            if ((float)tex->getWidth() / (float)tex->getHeight() * maxRegion.y > maxRegion.x) {
                // height is too small
                width = maxRegion.x;
                height = (float)tex->getHeight() / (float)tex->getWidth() * maxRegion.x;
            } else {
                height = maxRegion.y;
                width = (float)tex->getWidth() / (float)tex->getHeight() * maxRegion.y;
            }


            // im gui image location finder trick
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

            if (ImGui::BeginChild("SimView")) {
                in_foreground = in_foreground || ImGui::IsWindowFocused();

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
    }
    ImGui::End();

    this->simulation_visualization_window_in_foreground = in_foreground;
}

void FluidUi::FluidSolverWindow::setup_windows() {
    uiLayer.window = this;
    uiLayer.initialize();
}

void FluidUi::FluidSolverWindow::execute_one_simulation_step() {
    simulation.simulation->check_for_initialization();

    auto compatibility_report = simulation.simulation->check();
    if (compatibility_report.has_issues()) {
        compatibility_report.log_issues();
        running = false;
        return;
    }

    simulation.simulation->execute_simulation_step();
    simulation_changed_compared_to_visualization = true;
}

void FluidUi::FluidSolverWindow::visualize_simulation(bool called_from_worker_thread) {
    if (!simulation_changed_compared_to_visualization)
        return;

    simulation.simulation->check_for_initialization();

    auto compatibility_report = simulation.simulation->check();
    if (compatibility_report.has_issues()) {
        compatibility_report.log_issues();
        simulation_changed_compared_to_visualization = false;
        return;
    }

    bool is_gl_renderer =
            std::dynamic_pointer_cast<FluidSolver::GLRenderer>(simulation.visualizer) != nullptr;
    if (called_from_worker_thread && is_gl_renderer) {
        // an opengl renderer has to be called synchronously in the thread of the windows,
        // since the window owns the required gl context
        return;
    }


    // visualize the scene
    simulation.visualizer->render();

    if (!is_gl_renderer && simulation.visualizer != nullptr) {
        // if the visualizer is not an opengl renderer, copy the visualized image to be able to update it in another
        // thread
        render_image_copy = simulation.visualizer->get_image_data();
    }

    // after we are done with the work set the render_image_update flag to true
    render_image_updated = true;

    // the simulation state is now again in sync with the visualization
    simulation_changed_compared_to_visualization = false;
}

void FluidUi::FluidSolverWindow::sim_worker_thread_main() {
    using namespace std::chrono_literals;
    while (!sim_worker_thread_should_terminate) {
        if (!running || !asynchronous_simulation) {
            // only work if the simulation is running and if it is set to run asynchronously
            std::this_thread::sleep_for(100ms);
        } else {
            if (sim_worker_status == SimWorkerThreadStatus::SimWorkerThreadStatusWork) {
                // the simulation worker thread should do work

                // execute one simulation step
                execute_one_simulation_step();

                // visualize this simulation step if possible
                visualize_simulation(true);

                // state that the thread is done
                sim_worker_status = SimWorkerThreadStatus::SimWorkerThreadStatusDone;
            }
        }
    }
}

bool FluidUi::FluidSolverWindow::is_done_working() const {
    return sim_worker_status != SimWorkerThreadStatus::SimWorkerThreadStatusWork;
}

bool FluidUi::FluidSolverWindow::is_safe_to_access_simulation_data() const {
    return !asynchronous_simulation ||
            (!running && sim_worker_status != SimWorkerThreadStatus::SimWorkerThreadStatusWork);
}

void FluidUi::FluidSolverWindow::update_camera() {
    constexpr float CAM_SPEED = 5.0f;

    if (!simulation_visualization_window_in_foreground) {
        current_camera_movement = MovementDirection::NoMovement;
        return;
    }

    if (simulation.visualizer == nullptr)
        return;

    if (current_camera_movement == MovementDirection::NoMovement)
        return;

    auto gl = std::dynamic_pointer_cast<FluidSolver::GLParticleRenderer>(simulation.visualizer);
    auto cv = std::dynamic_pointer_cast<FluidSolver::ContinousVisualizer>(simulation.visualizer);
    auto gl3d = std::dynamic_pointer_cast<FluidSolver::GLParticleRenderer3D>(simulation.visualizer);

    if (gl != nullptr) {
        if (current_camera_movement == MovementDirection::Top) {
            gl->settings.viewport.bottom += get_last_frame_time() * CAM_SPEED;
            gl->settings.viewport.top += get_last_frame_time() * CAM_SPEED;
        } else if (current_camera_movement == MovementDirection::Bottom) {
            gl->settings.viewport.bottom -= get_last_frame_time() * CAM_SPEED;
            gl->settings.viewport.top -= get_last_frame_time() * CAM_SPEED;
        } else if (current_camera_movement == MovementDirection::Left) {
            gl->settings.viewport.left -= get_last_frame_time() * CAM_SPEED;
            gl->settings.viewport.right -= get_last_frame_time() * CAM_SPEED;
        } else if (current_camera_movement == MovementDirection::Right) {
            gl->settings.viewport.left += get_last_frame_time() * CAM_SPEED;
            gl->settings.viewport.right += get_last_frame_time() * CAM_SPEED;
        }
    }

    if (cv != nullptr) {
        if (current_camera_movement == MovementDirection::Top) {
            cv->settings.viewport.bottom += get_last_frame_time() * CAM_SPEED;
            cv->settings.viewport.top += get_last_frame_time() * CAM_SPEED;
        } else if (current_camera_movement == MovementDirection::Bottom) {
            cv->settings.viewport.bottom -= get_last_frame_time() * CAM_SPEED;
            cv->settings.viewport.top -= get_last_frame_time() * CAM_SPEED;
        } else if (current_camera_movement == MovementDirection::Left) {
            cv->settings.viewport.left -= get_last_frame_time() * CAM_SPEED;
            cv->settings.viewport.right -= get_last_frame_time() * CAM_SPEED;
        } else if (current_camera_movement == MovementDirection::Right) {
            cv->settings.viewport.left += get_last_frame_time() * CAM_SPEED;
            cv->settings.viewport.right += get_last_frame_time() * CAM_SPEED;
        }
    }

    if (gl3d != nullptr) {
        if (current_camera_movement == MovementDirection::Top) {
            gl3d->settings.camera.move_forward(get_last_frame_time() * CAM_SPEED);
        } else if (current_camera_movement == MovementDirection::Bottom) {
            gl3d->settings.camera.move_forward(-get_last_frame_time() * CAM_SPEED);
        } else if (current_camera_movement == MovementDirection::Left) {
            gl3d->settings.camera.move_right(-get_last_frame_time() * CAM_SPEED);
        } else if (current_camera_movement == MovementDirection::Right) {
            gl3d->settings.camera.move_right(get_last_frame_time() * CAM_SPEED);
        }
    }


    this->visualizer_parameter_changed();
}


void FluidUi::FluidSolverWindow::drag_viewport(double newX, double newY) {
    if (!is_dragging_viewport) {
        return;
    }
    if (!simulation_visualization_window_in_foreground) {
        is_dragging_viewport = false;
        return;
    }

    // calculate the delta
    double deltaX = newX - last_drag_location.x;
    double deltaY = newY - last_drag_location.y;

    last_drag_location = {newX, newY};

    // update the viewport accordingly
    if (simulation.visualizer == nullptr)
        return;

    auto gl = std::dynamic_pointer_cast<FluidSolver::GLParticleRenderer>(simulation.visualizer);
    auto cv = std::dynamic_pointer_cast<FluidSolver::ContinousVisualizer>(simulation.visualizer);
    auto gl3d = std::dynamic_pointer_cast<FluidSolver::GLParticleRenderer3D>(simulation.visualizer);


    if (gl != nullptr) {
        float camera_speed_x = gl->settings.viewport.width() / visualizer_window_size.width;
        float camera_speed_y = gl->settings.viewport.height() / visualizer_window_size.height;

        gl->settings.viewport.bottom += deltaY * camera_speed_y;
        gl->settings.viewport.top += deltaY * camera_speed_y;
        gl->settings.viewport.left -= deltaX * camera_speed_x;
        gl->settings.viewport.right -= deltaX * camera_speed_x;
    }

    if (cv != nullptr) {
        float camera_speed_x = cv->settings.viewport.width() / visualizer_window_size.width;
        float camera_speed_y = cv->settings.viewport.height() / visualizer_window_size.height;

        cv->settings.viewport.bottom += deltaY * camera_speed_y;
        cv->settings.viewport.top += deltaY * camera_speed_y;
        cv->settings.viewport.left -= deltaX * camera_speed_x;
        cv->settings.viewport.right -= deltaX * camera_speed_x;
    }

    if (gl3d != nullptr) {
        constexpr float CAM_SPEED = 0.01f;

        float camera_speed_x = gl3d->parameters.render_target.width / visualizer_window_size.width * CAM_SPEED;
        float camera_speed_y = gl3d->parameters.render_target.height / visualizer_window_size.height * CAM_SPEED;

        gl3d->settings.camera.rotate_horizontal(camera_speed_x * -deltaX);
        gl3d->settings.camera.rotate_vertical(camera_speed_y * -deltaY);
    }


    this->visualizer_parameter_changed();
}


void FluidUi::FluidSolverWindow::on_cursor_position_changed(double xpos, double ypos) {
    if (is_dragging_viewport) {
        this->drag_viewport(xpos, ypos);
    }
}
void FluidUi::FluidSolverWindow::on_mouse_down(MouseButton button) {
    if (button == MouseButton::LeftButton) {
        left_mouse_button_down = true;
        if (simulation_visualization_window_in_foreground) {
            is_dragging_viewport = true;
            last_drag_location = {get_mouse_position_x(), get_mouse_position_y()};
        }
    }
}
void FluidUi::FluidSolverWindow::on_mouse_up(MouseButton button) {
    if (button == MouseButton::LeftButton) {
        left_mouse_button_down = false;
        is_dragging_viewport = false;
    }
}

void FluidUi::FluidSolverWindow::on_key_pressed(int key) {
    if (key == GLFW_KEY_SPACE && simulation_visualization_window_in_foreground) {
        running = !running;
    }

    if (simulation_visualization_window_in_foreground) {
        if (key == GLFW_KEY_W) {
            current_camera_movement = MovementDirection::Top;
        } else if (key == GLFW_KEY_S) {
            current_camera_movement = MovementDirection::Bottom;
        } else if (key == GLFW_KEY_A) {
            current_camera_movement = MovementDirection::Left;
        } else if (key == GLFW_KEY_D) {
            current_camera_movement = MovementDirection::Right;
        }
    }
}
void FluidUi::FluidSolverWindow::on_key_released(int key) {
    if (simulation_visualization_window_in_foreground) {
        if (key == GLFW_KEY_W && current_camera_movement == MovementDirection::Top) {
            current_camera_movement = MovementDirection::NoMovement;
        } else if (key == GLFW_KEY_S && current_camera_movement == MovementDirection::Bottom) {
            current_camera_movement = MovementDirection::NoMovement;
        } else if (key == GLFW_KEY_A && current_camera_movement == MovementDirection::Left) {
            current_camera_movement = MovementDirection::NoMovement;
        } else if (key == GLFW_KEY_D && current_camera_movement == MovementDirection::Right) {
            current_camera_movement = MovementDirection::NoMovement;
        }
    }
}
