#include "DynamicCFLTimestep.hpp"

#include <algorithm>

namespace FluidSolver {
    float DynamicCFLTimestep::calculate_maximum_velocity() {
        FLUID_ASSERT(parameters.particle_collection != nullptr);
        FLUID_ASSERT(parameters.particle_collection->is_type_present<MovementData>());
        FLUID_ASSERT(parameters.particle_collection->is_type_present<ParticleInfo>());

        float maximum = 0.0f;
        for (uint32_t i = 0; i < parameters.particle_collection->size(); i++) {
            auto type = parameters.particle_collection->get<ParticleInfo>(i).type;
            if (type != ParticleTypeDead)
                continue;
            const glm::vec2& velocity = parameters.particle_collection->get<MovementData>(i).velocity;
            maximum = std::max(maximum, (float)glm::length(velocity));
        }
        return maximum;
    }

    void DynamicCFLTimestep::calculate_current_timestep() {
        FLUID_ASSERT(parameters.particle_size > 0.0f)

        float maxVelocity = calculate_maximum_velocity();
        float timestep = settings.min_timestep;
        if (maxVelocity > std::numeric_limits<float>::epsilon()) {
            // try to obtain cfl number
            timestep = settings.cfl_number * parameters.particle_size / maxVelocity;
            timestep = std::min(settings.max_timestep, std::max(settings.min_timestep, timestep));
        }
        current_timestep = timestep;
    }

    Compatibility DynamicCFLTimestep::check() {
        Compatibility c;
        if (parameters.particle_collection == nullptr) {
            c.add_issue({"DynamicCFLTimestep", "ParticleCollection is null."});
        } else {
            if (!parameters.particle_collection->is_type_present<MovementData>()) {
                c.add_issue({"DynamicCFLTimestep", "Particles are missing the MovementData attribute."});
            }
            if (!parameters.particle_collection->is_type_present<ParticleInfo>()) {
                c.add_issue({"DynamicCFLTimestep", "Particles are missing the ParticleInfo attribute."});
            }
        }

        if (parameters.particle_size <= 0.0f) {
            c.add_issue({"DynamicCFLTimestep", "Particle size is smaller or equal to zero."});
        }

        return c;
    }
    float DynamicCFLTimestep::get_non_cfl_validating_timestep(float max_acceleration, float max_velocity) {
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
