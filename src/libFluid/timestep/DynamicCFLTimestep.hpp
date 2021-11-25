#pragma once

#include "timestep/ITimestep.hpp"

namespace FluidSolver {
    class DynamicCFLTimestep : public ITimestep {
      public:
        struct DynamicCFLTimestepSettings {
            float max_timestep = 0.02f;
            float min_timestep = 0.0001f;
            float cfl_number = 0.8f;

            float lambda_v = 0.4f;
            float lambda_a = 0.25f;
        } settings;

        void calculate_current_timestep() override;

        Compatibility check() override;
        float get_non_cfl_validating_timestep(float max_acceleration, float max_velocity) override;

      private:
        float calculate_maximum_velocity();
    };
} // namespace FluidSolver
