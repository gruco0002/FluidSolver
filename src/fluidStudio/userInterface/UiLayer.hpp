#ifndef FLUIDSOLVER_UILAYER_HPP
#define FLUIDSOLVER_UILAYER_HPP

#include "StatisticsUi.hpp"
#include "sensors/SensorPlane.hpp"
#include "userInterface/LogWindow.hpp"

namespace FluidUi
{
    class FluidSolverWindow;

    class UiLayer {
      private:
        struct Component
        {
            enum class Kind
            {
                None,
                Solver,
                Visualizer,
                Timestep,
                Sensor,
                Output
            } kind = Kind::None;
            size_t index = 0;

            bool operator==(const Component& other) const;

            bool operator!=(const Component& other) const;

            bool can_delete() const;

        } selection;

      public:
        FluidSolverWindow* window = nullptr;
        void render();
        void initialize();

      private:
        StatisticsUi statisticsUi;
        LogWindow logWindow;

        void render_component_panel();
        void render_simulation_controls();
        void render_menu();
        void render_component_properties_panel();

        void render_component_node(const char* name, const Component& component);
        void render_component_settings(const Component& component);
        void render_solver_component();
        void render_solver_parameters();
        void render_timestep_component();
        void render_sensor_component(size_t index);
        void render_output_component();
        void render_visualizer_component();


        void render_global_energy_sensor_component(std::shared_ptr<FluidSolver::Sensors::GlobalEnergySensor> sen);
        void render_sensor_plane_component(std::shared_ptr<FluidSolver::Sensors::SensorPlane> sen);

        void delete_component(const Component& component);
    };

} // namespace FluidUi


#endif // FLUIDSOLVER_UILAYER_HPP
