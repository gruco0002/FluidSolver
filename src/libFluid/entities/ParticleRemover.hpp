#pragma once

#include "IEntity.hpp"

namespace FluidSolver {
    class ParticleRemover : public IEntity {
      public:
        struct Parameters {
            Area area = {};
            bool remove_if_outside = true;
        } parameters;

        void execute_simulation_step(pFloat timestep) override;
    };

} // namespace FluidSolver
