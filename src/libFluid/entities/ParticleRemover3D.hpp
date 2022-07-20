#pragma once

#include "IEntity.hpp"

namespace FluidSolver {
    class ParticleRemover3D : public IEntity {
      public:
        struct Parameters {
            Volume volume = {};
            bool remove_if_outside = true;
        } parameters;

        void execute_simulation_step(pFloat timestep) override;

        void initialize() override;
        void create_compatibility_report(CompatibilityReport& report) override;
    };

} // namespace FluidSolver
