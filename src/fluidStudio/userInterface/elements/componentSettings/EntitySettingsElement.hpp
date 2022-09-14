#pragma once

#include "entities/IEntity.hpp"
#include "userInterface/UiElement.hpp"

namespace FluidStudio {
    class EntitySettingsElement final : public UiElement {
      public:
        void update() override;

      private:
        void update_particle_remover_3d_component(std::shared_ptr<LibFluid::IEntity> tmp);
    };
} // namespace FluidStudio
