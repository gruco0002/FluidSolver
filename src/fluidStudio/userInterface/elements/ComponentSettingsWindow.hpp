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
        void render_component_settings(const Component& component);


        void render_solver_component();
        void render_solver_parameters();
        void render_timestep_component();
        void render_sensor_component(size_t index);
        void render_entity_component(size_t index);
        void render_output_component();
        void render_visualizer_component();
        void render_global_energy_sensor_component(std::shared_ptr<LibFluid::Sensors::GlobalEnergySensor> sen);
        void render_sensor_plane_component(std::shared_ptr<LibFluid::Sensors::SensorPlane> sen);
        void render_particle_remover_3d_component(std::shared_ptr<LibFluid::ParticleRemover3D> ent);
    };
} // namespace FluidStudio
