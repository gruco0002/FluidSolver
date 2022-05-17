#pragma once

#include "time/ITimestepGenerator.hpp"

namespace FluidSolver {
    class ConstantTimestepGenerator : public ITimestepGenerator {
      public:
        struct ConstantTimestepGeneratorSettings {
            float timestep = 0.001f;

            float lambda_v = 0.4f;
            float lambda_a = 0.25f;
        } settings;

        float get_non_cfl_validating_timestep(float max_acceleration, float max_velocity) override;
        void generate_next_timestep() override;

        void create_compatibility_report(CompatibilityReport &report) override;
    };
} // namespace FluidSolver
