#pragma once

#include "engine/graphics/Texture2D.hpp"
#include "sensors/CompressedNeighborsStatistics.hpp"
#include "sensors/ParticleStatistics.hpp"
#include "sensors/SensorPlane.hpp"

#include <memory>
#include <vector>

namespace FluidUi {
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

        Engine::Graphics::Texture2D* rendered_image = nullptr;

        bool* sensor_window_open_ptr(size_t index);

        void render_density_sensor_graph(std::shared_ptr<FluidSolver::Sensors::GlobalDensitySensor> sensor);
        void render_pressure_sensor_graph(std::shared_ptr<FluidSolver::Sensors::GlobalPressureSensor> sensor);
        void render_velocity_sensor_graph(std::shared_ptr<FluidSolver::Sensors::GlobalVelocitySensor> sensor);
        void render_energy_sensor_graph(std::shared_ptr<FluidSolver::Sensors::GlobalEnergySensor> sensor);
        void render_particle_count_sensor(std::shared_ptr<FluidSolver::Sensors::GlobalParticleCountSensor> sensor);
        void render_sensor_plane_sensor(std::shared_ptr<FluidSolver::Sensors::SensorPlane> sensor);
        void render_compressed_neighborhood_storage_sensor(
                std::shared_ptr<FluidSolver::Sensors::CompressedNeighborStorageSensor> sensor);
        void render_iisph_sensor(std::shared_ptr<FluidSolver::Sensors::IISPHSensor> sensor);
    };

} // namespace FluidUi
