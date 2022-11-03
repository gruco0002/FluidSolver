#pragma once

#include "entities/SimulationEntity.hpp"
#include "group/Area.hpp"

namespace LibFluid {
    class ParticleRemover : public SimulationEntity {
      public:
        struct Parameters {
            Area area = {};
            bool remove_if_outside = true;
        } parameters;

        void execute_simulation_step(const Timepoint& timepoint, bool before_solver) override;
        void initialize() override;
        void create_compatibility_report(CompatibilityReport& report) override;
    };

} // namespace FluidSolver
