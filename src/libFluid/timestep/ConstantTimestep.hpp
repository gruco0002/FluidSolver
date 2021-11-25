#pragma once

#include "timestep/ITimestep.hpp"

namespace FluidSolver {
    class ConstantTimestep : public ITimestep {
      public:
        struct ConstantTimestepSettings {
            float timestep = 0.001f;

            float lambda_v = 0.4f;
            float lambda_a = 0.25f;
        } settings;

        float get_non_cfl_validating_timestep(float max_acceleration, float max_velocity) override;
        void calculate_current_timestep() override;

        Compatibility check() override;
    };
} // namespace FluidSolver
