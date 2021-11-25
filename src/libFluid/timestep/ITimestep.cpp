#include "ITimestep.hpp"
namespace FluidSolver {

    float ITimestep::get_current_timestep() const {
        return current_timestep;
    }

    float Timepoint::get_non_cfl_validating_timestep(float max_velocity, float particle_size) {
        // in order for the Courant–Friedrichs–Lewy condition to be true the following must hold:
        // particle_size > max_velocity * timestep

        constexpr float downscale_factor = 0.001f;
        constexpr float downscale_multiplier = 1.0f - downscale_factor;

        FLUID_ASSERT(max_velocity > 0.0f);
        FLUID_ASSERT(particle_size > 0.0f);

        return particle_size * downscale_multiplier / max_velocity;
    }

} // namespace FluidSolver
