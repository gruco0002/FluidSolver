#pragma once

#include "timestep/ITimestepGenerator.hpp"

#include <tuple>

namespace FluidSolver {
    class DynamicCFLTimestep : public ITimestepGenerator {
      public:
        struct DynamicCFLTimestepSettings {
            float max_timestep = 0.02f;
            float min_timestep = 0.0001f;
            float cfl_number = 0.8f;

            float lambda_v = 0.4f;
            float lambda_a = 0.25f;
        } settings;

        void generate_next_timestep() override;

        Compatibility check() override;
        float get_non_cfl_validating_timestep(float max_acceleration, float max_velocity) override;

      private:
        std::tuple<float, float> calculate_maximum_velocity_and_acceleration();
    };
} // namespace FluidSolver
