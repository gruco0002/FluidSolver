#pragma once

#include "SimulationEntity.hpp"

namespace LibFluid {
    class BoundaryPreprocessor final : public SimulationEntity {
      public:
        BoundaryPreprocessor();

        void execute_simulation_step(const Timepoint& timepoint, bool before_solver) override;
        void initialize() override;
        void create_compatibility_report(CompatibilityReport& report) override;
    };
} // namespace LibFluid
