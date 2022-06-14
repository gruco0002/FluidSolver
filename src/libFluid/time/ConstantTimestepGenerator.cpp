#include "ConstantTimestepGenerator.hpp"

namespace FluidSolver {
    void ConstantTimestepGenerator::generate_next_timestep() {
        initialize();
        generated_timestep = settings.timestep;
    }

    void ConstantTimestepGenerator::create_compatibility_report(CompatibilityReport& report) {
        initialize();
        report.begin_scope(FLUID_NAMEOF(ConstantTimestepGenerator));
        if (settings.timestep <= 0.0f) {
            report.add_issue("Timestep is smaller or equal to zero.");
        }
        report.end_scope();
    }
    float ConstantTimestepGenerator::get_non_cfl_validating_timestep(float max_acceleration, float max_velocity) {
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
    void ConstantTimestepGenerator::initialize() {
    }


} // namespace FluidSolver
