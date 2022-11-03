#pragma once

#include "sensors/Sensor.hpp"
#include "userInterface/UiElement.hpp"

namespace FluidStudio {

    class SensorSettingsElement final : public UiElement {
      public:
        void update() override;

      private:
        void update_global_energy_sensor_component(std::shared_ptr<LibFluid::Sensor> sen);
        void update_sensor_plane_component(std::shared_ptr<LibFluid::Sensor> sen);
        void update_particle_count_sensor(std::shared_ptr<LibFluid::Sensor> sen);
    };

} // namespace FluidStudio
