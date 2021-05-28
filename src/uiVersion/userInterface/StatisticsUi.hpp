#ifndef FLUIDSOLVER_STATISTICSUI_HPP
#define FLUIDSOLVER_STATISTICSUI_HPP

#include <core/sensors/ParticleStatistics.hpp>
#include <vector>

namespace FluidUi
{
    class FluidSolverWindow;

    class StatisticsUi {

      public:
        FluidSolverWindow* window = nullptr;

        void render();
        void initialize();

        void open_sensor_window(size_t index);

        bool is_sensor_window_open(size_t index);

      private:
        std::vector<uint8_t> open_sensor_windows;

        bool* sensor_window_open_ptr(size_t index);

        void render_density_sensor_graph(FluidSolver::Sensors::GlobalDensitySensor* sensor);
        void render_pressure_sensor_graph(FluidSolver::Sensors::GlobalPressureSensor* sensor);
        void render_velocity_sensor_graph(FluidSolver::Sensors::GlobalVelocitySensor* sensor);
        void render_energy_sensor_graph(FluidSolver::Sensors::GlobalEnergySensor* sensor);
        void render_particle_count_sensor(FluidSolver::Sensors::GlobalParticleCountSensor* sensor);
    };

} // namespace FluidUi


#endif // FLUIDSOLVER_STATISTICSUI_HPP
