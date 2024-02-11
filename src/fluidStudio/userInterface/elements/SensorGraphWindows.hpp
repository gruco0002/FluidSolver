#pragma once

#include "engine/graphics/Texture2D.hpp"
#include "sensors/CompressedNeighborsStatistics.hpp"
#include "sensors/ParticleStatistics.hpp"
#include "sensors/SensorPlane.hpp"

#include "userInterface/UiElement.hpp"

#include <memory>
#include <vector>

namespace FluidStudio
{

    class SensorGraphWindows final : public UiElement
    {
      public:
        void update() override;

        void open_sensor_window(size_t index);
        bool is_sensor_window_open(size_t index);

      private:
        std::vector<uint8_t> open_sensor_windows;

        Engine::Graphics::Texture2D *rendered_image = nullptr;

        bool *sensor_window_open_ptr(size_t index);

        void render_density_sensor_graph(std::shared_ptr<LibFluid::Sensors::GlobalDensitySensor> sensor);
        void render_pressure_sensor_graph(std::shared_ptr<LibFluid::Sensors::GlobalPressureSensor> sensor);
        void render_velocity_sensor_graph(std::shared_ptr<LibFluid::Sensors::GlobalVelocitySensor> sensor);
        void render_energy_sensor_graph(std::shared_ptr<LibFluid::Sensors::GlobalEnergySensor> sensor);
        void render_particle_count_sensor(std::shared_ptr<LibFluid::Sensors::GlobalParticleCountSensor> sensor);
        void render_sensor_plane_sensor(std::shared_ptr<LibFluid::Sensors::SensorPlane> sensor);
        void render_compressed_neighborhood_storage_sensor(
            std::shared_ptr<LibFluid::Sensors::CompressedNeighborStorageSensor> sensor);
        void render_iisph_sensor(std::shared_ptr<LibFluid::Sensors::IISPHSensor> sensor);
    };

} // namespace FluidStudio
