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

namespace FluidUi {

    FluidSolverWindow::FluidSolverWindow(const std::string& title, int width, int height)
        : Window(title, width, height),
          render_image_copy(0, 0) {
    }

    void FluidSolverWindow::load() {
        // set the current fluid solver type and create an empty simulation
        current_type = &solver_types.types[0];
        create_empty_simulation();

        // initialize imgui and window
        ImGuiHelper::Init(this->get_window_handler());

        // setup the windows
        setup_ui_layer();

        // set the initial size of the windows
        set_width((int)((float)get_screen_width() * 0.9f));
        set_height((int)((float)get_screen_height() * 0.9f));
    }

    void FluidSolverWindow::unload() {
        ImGuiHelper::Uninit();

        delete rendered_image;
        rendered_image = nullptr;
    }

    void FluidSolverWindow::render() {
        // simulating and rendering the visualization of the simulation
        {
            // update the runners
            if (simulation_runner.simulation != simulator_visualizer_bundle.simulator) {
                simulation_runner.simulation = simulator_visualizer_bundle.simulator;
            }
            if (visualization_runner.visualizer != simulator_visualizer_bundle.visualizer) {
                visualization_runner.visualizer = simulator_visualizer_bundle.visualizer;
            }

            // set the current running mode
            simulation_runner.run_asynchronously = asynchronous_simulation;
            visualization_runner.run_asynchronously = asynchronous_simulation && !is_simulation_visualizer_instance_of_gl_renderer();

            if (running) {
                // check if we are currently rendering data
                if (!visualization_runner.is_currently_computing()) {
                    // starting a computation if possible (if not, nothing will happen)
                    simulation_runner.start_next_computation();
                }
            }

            // if the simulation has made a step, we need to update the rendered image and reset the runner
            if (simulation_runner.is_done()) {
                simulation_changed_compared_to_visualization = true;
                simulation_runner.reset_runner_on_done();
            }

            if (simulation_changed_compared_to_visualization) {
                // simulation data changed, hence try to rerender the data
                if (!simulation_runner.is_currently_computing()) {
                    // only render if the data is not accessed by the simulation runner
                    if (visualization_runner.start_next_computation()) {
                        // the start was successfull
                        // the simulation state will be in sync with the visualization
                        simulation_changed_compared_to_visualization = false;
                    }
                }
            }

            // if the visualizer is done, copy the rendered image if required and reset the runner
            if (visualization_runner.is_done()) {
                if (!is_simulation_visualizer_instance_of_gl_renderer() && simulator_visualizer_bundle.visualizer != nullptr) {
                    // if the visualizer is not an opengl renderer, copy the visualized image to be able to
                    // access it in the main thread while the visualizer can start the next computation
                    render_image_copy = simulator_visualizer_bundle.visualizer->get_image_data();

                    // state the update of the render image
                    render_image_copy_updated = true;
                }

                // reset the runner to make it available for the next computation
                visualization_runner.reset_runner_on_done();
            }
        }

        // rendering stuff to the window (ui, ...)
        {
            // render to screen
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glViewport(0, 0, get_framebuffer_width(), get_framebuffer_height());

            ImGuiHelper::PreRender();

            // creating the dockspace
            auto id = ImGui::DockSpaceOverViewport();

            render_visualization_ui_window();
            uiLayer.render();

            ImGuiHelper::PostRender();
        }


        // update the camera
        update_camera();
    }

    void FluidSolverWindow::create_empty_simulation() {
        simulator_visualizer_bundle.simulator = std::make_shared<FluidSolver::Simulator>();

        simulator_visualizer_bundle.simulator->data.collection = std::make_shared<FluidSolver::ParticleCollection>();
        simulator_visualizer_bundle.simulator->data.collection->add_type<FluidSolver::MovementData>();
        simulator_visualizer_bundle.simulator->data.collection->add_type<FluidSolver::ParticleData>();
        simulator_visualizer_bundle.simulator->data.collection->add_type<FluidSolver::ParticleInfo>();
        simulator_visualizer_bundle.simulator->data.collection->add_type<FluidSolver::ExternalForces>();
        simulator_visualizer_bundle.simulator->parameters.rest_density = 1.0f;
        simulator_visualizer_bundle.simulator->parameters.particle_size = 1.0f;
        simulator_visualizer_bundle.simulator->parameters.gravity = 9.81f;

        simulator_visualizer_bundle.simulator->data.fluid_solver = current_type->create_type();
        simulator_visualizer_bundle.simulator->data.timestep_generator = std::make_shared<FluidSolver::ConstantTimestepGenerator>();
        simulator_visualizer_bundle.visualizer = std::make_shared<FluidSolver::GLParticleRenderer>();

        // simulation.parameters.sensors.push_back(new FluidSolver::ParticleStatisticsSensor());

        simulator_visualizer_bundle.simulator->parameters.notify_that_data_changed();
        simulator_visualizer_bundle.simulator->data.notify_that_data_changed();


        FluidSolver::Log::message("Loaded empty scenario");

        on_new_simulation();
    }

    void FluidSolverWindow::create_empty_3d_simulation() {
        simulator_visualizer_bundle.simulator = std::make_shared<FluidSolver::Simulator>();

        simulator_visualizer_bundle.simulator->data.collection = std::make_shared<FluidSolver::ParticleCollection>();
        simulator_visualizer_bundle.simulator->data.collection->add_type<FluidSolver::MovementData3D>();
        simulator_visualizer_bundle.simulator->data.collection->add_type<FluidSolver::ParticleData>();
        simulator_visualizer_bundle.simulator->data.collection->add_type<FluidSolver::ParticleInfo>();
        simulator_visualizer_bundle.simulator->data.collection->add_type<FluidSolver::ExternalForces3D>();
        simulator_visualizer_bundle.simulator->parameters.rest_density = 1.0f;
        simulator_visualizer_bundle.simulator->parameters.particle_size = 1.0f;
        simulator_visualizer_bundle.simulator->parameters.gravity = 9.81f;

        simulator_visualizer_bundle.simulator->data.fluid_solver = current_type->create_type();
        simulator_visualizer_bundle.simulator->data.timestep_generator = std::make_shared<FluidSolver::ConstantTimestepGenerator>();
        simulator_visualizer_bundle.visualizer = std::make_shared<FluidSolver::GLParticleRenderer3D>();

        simulator_visualizer_bundle.simulator->parameters.notify_that_data_changed();
        simulator_visualizer_bundle.simulator->data.notify_that_data_changed();


        FluidSolver::Log::message("Loaded empty 3d scenario");

        on_new_simulation();
    }

    void FluidSolverWindow::create_3d_test_simulation() {
        simulator_visualizer_bundle.simulator = std::make_shared<FluidSolver::Simulator>();

        simulator_visualizer_bundle.simulator->data.collection = std::make_shared<FluidSolver::ParticleCollection>();
        simulator_visualizer_bundle.simulator->data.collection->add_type<FluidSolver::MovementData3D>();
        simulator_visualizer_bundle.simulator->data.collection->add_type<FluidSolver::ParticleData>();
        simulator_visualizer_bundle.simulator->data.collection->add_type<FluidSolver::ParticleInfo>();
        simulator_visualizer_bundle.simulator->data.collection->add_type<FluidSolver::ExternalForces3D>();
        simulator_visualizer_bundle.simulator->parameters.rest_density = 1.0f;
        simulator_visualizer_bundle.simulator->parameters.particle_size = 1.0f;
        simulator_visualizer_bundle.simulator->parameters.gravity = 9.81f;


        simulator_visualizer_bundle.simulator->data.fluid_solver = current_type->create_type();
        simulator_visualizer_bundle.simulator->data.timestep_generator = std::make_shared<FluidSolver::ConstantTimestepGenerator>();
        simulator_visualizer_bundle.visualizer = std::make_shared<FluidSolver::GLParticleRenderer3D>();

        simulator_visualizer_bundle.simulator->parameters.notify_that_data_changed();
        simulator_visualizer_bundle.simulator->data.notify_that_data_changed();

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
        simulator_visualizer_bundle.simulator->data.sensors.push_back(sen);


        for (int x = -5; x < 5; x++) {
            for (int y = -5; y < 5; y++) {
                for (int z = -5; z < 5; z++) {
                    auto index = simulator_visualizer_bundle.simulator->data.collection->add();
                    auto& pos = simulator_visualizer_bundle.simulator->data.collection->get<FluidSolver::MovementData3D>(index);
                    pos.position = glm::vec3((float)x, (float)y, (float)z);

                    auto& info = simulator_visualizer_bundle.simulator->data.collection->get<FluidSolver::ParticleInfo>(index);
                    info.type = FluidSolver::ParticleType::ParticleTypeNormal;

                    auto& data = simulator_visualizer_bundle.simulator->data.collection->get<FluidSolver::ParticleData>(index);
                    data.density = 1.0f;
                    data.mass = 1.0f;
                    data.pressure = 0.0f;
                }
            }
        }

        auto emit_boundary = [&](float x, float y, float z) {
            auto index = simulator_visualizer_bundle.simulator->data.collection->add();
            auto& pos = simulator_visualizer_bundle.simulator->data.collection->get<FluidSolver::MovementData3D>(index);
            pos.position = glm::vec3((float)x, (float)y, (float)z);

            auto& info = simulator_visualizer_bundle.simulator->data.collection->get<FluidSolver::ParticleInfo>(index);
            info.type = FluidSolver::ParticleType::ParticleTypeBoundary;

            auto& data = simulator_visualizer_bundle.simulator->data.collection->get<FluidSolver::ParticleData>(index);
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


    void FluidSolverWindow::on_new_simulation() {
        // determine the current solver type and set it
        this->current_type = this->solver_types.query_type(simulator_visualizer_bundle.simulator->data.fluid_solver);

        // initialize the simulation
        simulator_visualizer_bundle.simulator->manual_initialize();

        // create a compatibility report to showcase any errors
        FluidSolver::CompatibilityReport report;
        simulator_visualizer_bundle.create_compatibility_report(report);
        if (report.has_issues()) {
            report.log_issues();
        }

        // state that the simulation needs to be rerendered
        simulation_changed_compared_to_visualization = true;
    }

    void FluidSolverWindow::visualizer_parameter_changed() {
        simulation_changed_compared_to_visualization = true;
    }

    void FluidSolverWindow::render_visualization_ui_window() {
        bool in_foreground = false;

        ImGui::Begin("Simulation Visualization");

        in_foreground = in_foreground || ImGui::IsWindowFocused();

        auto glRenderer = std::dynamic_pointer_cast<FluidSolver::GLRenderer>(simulator_visualizer_bundle.visualizer);
        if (simulator_visualizer_bundle.visualizer == nullptr) {
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

                if (render_image_copy_updated) {
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

                    render_image_copy_updated = false;
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

        this->simulation_visualization_ui_window_in_foreground = in_foreground;
    }

    void FluidSolverWindow::setup_ui_layer() {
        uiLayer.window = this;
        uiLayer.initialize();
    }


    void FluidSolverWindow::update_camera() {
        constexpr float CAM_SPEED = 5.0f;

        if (!simulation_visualization_ui_window_in_foreground) {
            current_camera_movement = MovementDirection::NoMovement;
            return;
        }

        if (simulator_visualizer_bundle.visualizer == nullptr)
            return;

        if (current_camera_movement == MovementDirection::NoMovement)
            return;

        auto gl = std::dynamic_pointer_cast<FluidSolver::GLParticleRenderer>(simulator_visualizer_bundle.visualizer);
        auto cv = std::dynamic_pointer_cast<FluidSolver::ContinousVisualizer>(simulator_visualizer_bundle.visualizer);
        auto gl3d = std::dynamic_pointer_cast<FluidSolver::GLParticleRenderer3D>(simulator_visualizer_bundle.visualizer);

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


    void FluidSolverWindow::drag_viewport(double newX, double newY) {
        if (!is_dragging_viewport) {
            return;
        }
        if (!simulation_visualization_ui_window_in_foreground) {
            is_dragging_viewport = false;
            return;
        }

        // calculate the delta
        double deltaX = newX - last_drag_location.x;
        double deltaY = newY - last_drag_location.y;

        last_drag_location = {newX, newY};

        // update the viewport accordingly
        if (simulator_visualizer_bundle.visualizer == nullptr)
            return;

        auto gl = std::dynamic_pointer_cast<FluidSolver::GLParticleRenderer>(simulator_visualizer_bundle.visualizer);
        auto cv = std::dynamic_pointer_cast<FluidSolver::ContinousVisualizer>(simulator_visualizer_bundle.visualizer);
        auto gl3d = std::dynamic_pointer_cast<FluidSolver::GLParticleRenderer3D>(simulator_visualizer_bundle.visualizer);


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


    void FluidSolverWindow::on_cursor_position_changed(double xpos, double ypos) {
        if (is_dragging_viewport) {
            this->drag_viewport(xpos, ypos);
        }
    }
    void FluidSolverWindow::on_mouse_down(MouseButton button) {
        if (button == MouseButton::LeftButton) {
            left_mouse_button_down = true;
            if (simulation_visualization_ui_window_in_foreground) {
                is_dragging_viewport = true;
                last_drag_location = {get_mouse_position_x(), get_mouse_position_y()};
            }
        }
    }
    void FluidSolverWindow::on_mouse_up(MouseButton button) {
        if (button == MouseButton::LeftButton) {
            left_mouse_button_down = false;
            is_dragging_viewport = false;
        }
    }

    void FluidSolverWindow::on_key_pressed(int key) {
        if (key == GLFW_KEY_SPACE && simulation_visualization_ui_window_in_foreground) {
            running = !running;
        }

        if (simulation_visualization_ui_window_in_foreground) {
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
    void FluidSolverWindow::on_key_released(int key) {
        if (simulation_visualization_ui_window_in_foreground) {
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
    bool FluidSolverWindow::is_simulation_visualizer_instance_of_gl_renderer() const {
        return std::dynamic_pointer_cast<FluidSolver::GLRenderer>(simulator_visualizer_bundle.visualizer) != nullptr;
    }

    bool FluidSolverWindow::is_safe_to_access_simulation_data() const {
        if (!asynchronous_simulation)
            return true;

        return simulation_runner.is_ready() && visualization_runner.is_ready();
    }
} // namespace FluidUi