#pragma once

#include "entities/SimulationEntity.hpp"
#include "userInterface/UiElement.hpp"

namespace FluidStudio {
    class EntitySettingsElement final : public UiElement {
      public:
        void update() override;

      private:
        void update_particle_remover_3d_component(std::shared_ptr<LibFluid::SimulationEntity> tmp);
    };
} // namespace FluidStudio