#pragma once

#include "fluidSolver/Particle.hpp"
#include "userInterface/UiElement.hpp"

namespace FluidStudio
{
    class InsertParticlesWindow final : public UiElement
    {
      public:
        void update() override;

        void open_window();

      private:
        bool is_open = false;

        struct
        {
            size_t width = 10;
            size_t height = 10;
            size_t depth = 10;

            uint32_t tag = 0;
            LibFluid::ParticleType particle_type = LibFluid::ParticleTypeBoundary;

        } particle_settings;

        void reset();

        void insert_into_simulation();
    };
} // namespace FluidStudio
