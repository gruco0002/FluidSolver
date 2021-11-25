#include "ConstantTimestep.hpp"

namespace FluidSolver {
    void ConstantTimestep::calculate_current_timestep() {
        current_timestep = settings.timestep;
    }

    Compatibility ConstantTimestep::check() {
        Compatibility c;
        if (settings.timestep <= 0.0f) {
            c.add_issue({"ConstantTimestep", "Timestep is smaller or equal to zero."});
        }
        return c;
    }
    float ConstantTimestep::get_non_cfl_validating_timestep(float max_acceleration, float max_velocity) {
        FLUID_ASSERT(max_velocity > 0.0f);
        FLUID_ASSERT(max_acceleration > 0.0f);
        FLUID_ASSERT(parameters.particle_size > 0.0f);

        FLUID_ASSERT(settings.lambda_a > 0.0f);
        FLUID_ASSERT(settings.lambda_a < 1.0f);

        FLUID_ASSERT(settings.lambda_v > 0.0f);
        FLUID_ASSERT(settings.lambda_v < 1.0f);

        float delta_t_a = sqrt(parameters.particle_size / max_acceleration) * settings.lambda_a;
        float delta_t_v = parameters.particle_size / max_velocity * settings.lambda_v;

        return std::fmin(delta_t_a, delta_t_v);
    }


} // namespace FluidSolver
