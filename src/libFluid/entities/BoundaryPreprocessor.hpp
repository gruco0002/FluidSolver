#pragma once

#include "SimulationEntity.hpp"
#include "fluidSolver/kernel/CubicSplineKernel3D.hpp"

namespace LibFluid
{
    class BoundaryPreprocessor final : public SimulationEntity
    {
      public:
        BoundaryPreprocessor();

        CubicSplineKernel3D kernel;

        void execute_simulation_step(const Timepoint &timepoint, bool before_solver) override;
        void initialize() override;
        void create_compatibility_report(CompatibilityReport &report) override;

      private:
        float ideal_volume_reciprocal = 0.0f;
        float gamma_1 = 0.0f;
        float max_allowed_mass = 0.0f;

        float calculate_ideal_volume_reciprocal() const;
        float calculate_gamma_1_from_ideal_volume_reciprocal() const;
        float calculate_max_allowed_mass() const;
    };
} // namespace LibFluid
