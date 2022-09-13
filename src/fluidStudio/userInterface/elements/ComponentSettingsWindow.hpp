#pragma once

#include "userInterface/SimulationComponent.hpp"
#include "userInterface/UiElement.hpp"

#include "entities/ParticleRemover3D.hpp"
#include "sensors/ParticleStatistics.hpp"
#include "sensors/SensorPlane.hpp"

#include <memory>

namespace FluidStudio {
    class ComponentSettingsWindow : public UiElement {
      public:
        void update() override;

      private:
        void update_component_settings(const Component& component);


        void update_solver_component();
        void update_solver_parameters();
        void update_timestep_component();
        void update_sensor_component(size_t index);
        void update_entity_component(size_t index);
        void update_output_component();
        void update_visualizer_component();
        void update_global_energy_sensor_component(std::shared_ptr<LibFluid::Sensors::GlobalEnergySensor> sen);
        void update_sensor_plane_component(std::shared_ptr<LibFluid::Sensors::SensorPlane> sen);
        void update_particle_remover_3d_component(std::shared_ptr<LibFluid::ParticleRemover3D> ent);
    };
} // namespace FluidStudio
