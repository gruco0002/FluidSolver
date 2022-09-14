#pragma once

#include "userInterface/SimulationComponent.hpp"
#include "userInterface/UiElement.hpp"
#include "userInterface/elements/componentSettings/SensorSettingsElement.hpp"

#include "entities/ParticleRemover3D.hpp"


#include <memory>

namespace FluidStudio {
    class ComponentSettingsWindow : public UiElement {
      public:
        void update() override;
        void initialize(const UiData& ui_data) override;

      private:
        SensorSettingsElement sensor_settings_element;

        void update_component_settings(const Component& component);

        void update_solver_component();
        void update_solver_parameters();
        void update_timestep_component();
        void update_entity_component(size_t index);
        void update_output_component();
        void update_visualizer_component();

        void update_particle_remover_3d_component(std::shared_ptr<LibFluid::ParticleRemover3D> ent);
    };
} // namespace FluidStudio
