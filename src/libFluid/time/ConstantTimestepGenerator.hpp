#pragma once

#include "time/TimestepGenerator.hpp"

namespace LibFluid
{
    class ConstantTimestepGenerator : public TimestepGenerator
    {
      public:
        struct ConstantTimestepGeneratorSettings
        {
            float timestep = 0.001f;

            float lambda_v = 0.4f;
            float lambda_a = 0.25f;
        } settings;

        float get_non_cfl_validating_timestep(float max_acceleration, float max_velocity) override;
        void generate_next_timestep() override;

        void create_compatibility_report(CompatibilityReport &report) override;

        void initialize() override;
    };
} // namespace LibFluid
