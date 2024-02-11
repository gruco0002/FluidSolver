#pragma once

#include "userInterface/UiElement.hpp"

namespace FluidStudio
{
    class TagDescriptorSettingsElement final : public UiElement
    {
      public:
        void update() override;

      private:
        LibFluid::ParticleType particle_type = LibFluid::ParticleTypeNormal;

        void apply_type_to_particles(uint32_t particle_tag);
    };
} // namespace FluidStudio
