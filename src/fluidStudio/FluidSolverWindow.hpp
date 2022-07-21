#pragma once

#include "FluidSolverTypes.hpp"
#include "Simulator.hpp"
#include "SimulatorVisualizerBundle.hpp"
#include "engine/RectangleRenderer.hpp"
#include "engine/Window.hpp"
#include "engine/graphics/Framebuffer.hpp"
#include "userInterface/UiLayer.hpp"

#include "TimelineService.hpp"
#include "visualizationOverlay/VisualizationOverlay.hpp"
#include "runners/SimulationRunner.hpp"
#include "runners/VisualizationRunner.hpp"

namespace FluidUi {
    class FluidSolverWindow : public Engine::Window {
      public:
        explicit FluidSolverWindow(const std::string& title, int width = 800, int height = 600);

      protected:
        void unload() override;

        void render() override;

        void load() override;

      protected:
        void on_cursor_position_changed(double xpos, double ypos) override;
        void on_mouse_down(MouseButton button) override;
        void on_mouse_up(MouseButton button) override;
        void on_key_pressed(int key) override;
        void on_key_released(int key) override;


      public:
        FluidSolver::SimulatorVisualizerBundle simulator_visualizer_bundle;


        bool simulation_should_run = false;

        bool asynchronous_simulation = true;


        bool is_safe_to_access_simulation_data() const;
        bool are_calculations_running() const;

      public:
        // Loading and creating resources
        const FluidSolverTypes solver_types;

        const FluidSolverTypes::FluidSolverType* current_type = nullptr;

        void create_empty_simulation();

        void create_empty_3d_simulation();

        void create_3d_test_simulation();


        // other stuff below


        void on_new_simulation();

        void visualizer_parameter_changed();

      private:
        SimulationRunner simulation_runner;
        VisualizationRunner visualization_runner;

        bool is_simulation_visualizer_instance_of_gl_renderer() const;
        bool simulation_changed_compared_to_visualization = false;

      public:
        TimelineService timeline_service;
        VisualizationOverlay visualization_overlay;

      private:
        UiLayer uiLayer;
        void render_visualization_ui_window();
        void setup_ui_layer();
        bool simulation_visualization_ui_window_in_foreground = false;
        struct
        {
            float width;
            float height;
        } visualizer_window_size;



        void render_visualization_overlay(float visualization_width, float visualization_height);
        void render_visualization_overlay_into_framebuffer();

      private:
        // visualizer stuff
        FluidSolver::Image render_image_copy;
        bool render_image_copy_updated = false;
        Engine::Graphics::Texture2D* rendered_image = nullptr;


      private:
        // camera stuff
        enum class MovementDirection {
            Left,
            Right,
            Top,
            Bottom,
            NoMovement
        } current_camera_movement;

        void update_camera();

        bool left_mouse_button_down = false;
        bool is_dragging_viewport = false;

        struct
        {
            double x;
            double y;
        } last_drag_location;

        void drag_viewport(double newX, double newY);
    };
} // namespace FluidUi
