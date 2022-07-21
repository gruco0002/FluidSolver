#pragma once

#include "IEntity.hpp"

namespace LibFluid {
    class ParticleRemover : public IEntity {
      public:
        struct Parameters {
            Area area = {};
            bool remove_if_outside = true;
        } parameters;

        void execute_simulation_step(pFloat timestep) override;

        void initialize() override;
        void create_compatibility_report(CompatibilityReport& report) override;
    };

} // namespace FluidSolver
