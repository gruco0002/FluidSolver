#include "DynamicCflTimestepGenerator.hpp"

#include <algorithm>

namespace LibFluid {
    std::tuple<float, float> DynamicCflTimestepGenerator::calculate_maximum_velocity_and_acceleration() {
        FLUID_ASSERT(parameters.particle_collection != nullptr);
        FLUID_ASSERT(parameters.particle_collection->is_type_present<MovementData>() || parameters.particle_collection->is_type_present<MovementData3D>());
        FLUID_ASSERT(parameters.particle_collection->is_type_present<ParticleInfo>());

        float maximum_veloctiy = 0.0f;
        float maximum_acceleration = 0.0f;

        if (parameters.particle_collection->is_type_present<MovementData3D>()) {
            for (uint32_t i = 0; i < parameters.particle_collection->size(); i++) {
                auto type = parameters.particle_collection->get<ParticleInfo>(i).type;
                if (type == ParticleTypeInactive)
                    continue;


                const glm::vec3& velocity = parameters.particle_collection->get<MovementData3D>(i).velocity;
                const glm::vec3& acceleration = parameters.particle_collection->get<MovementData3D>(i).acceleration;

                maximum_veloctiy = std::max(maximum_veloctiy, (float)glm::length(velocity));
                maximum_acceleration = std::max(maximum_acceleration, (float)glm::length(acceleration));
            }
        } else {
            for (uint32_t i = 0; i < parameters.particle_collection->size(); i++) {
                auto type = parameters.particle_collection->get<ParticleInfo>(i).type;
                if (type == ParticleTypeInactive)
                    continue;


                const glm::vec2& velocity = parameters.particle_collection->get<MovementData>(i).velocity;
                const glm::vec2& acceleration = parameters.particle_collection->get<MovementData>(i).acceleration;

                maximum_veloctiy = std::max(maximum_veloctiy, (float)glm::length(velocity));
                maximum_acceleration = std::max(maximum_acceleration, (float)glm::length(acceleration));
            }
        }


        return {maximum_veloctiy, maximum_acceleration};
    }

    void DynamicCflTimestepGenerator::generate_next_timestep() {
        initialize();

        auto [max_velocity, max_acceleration] = calculate_maximum_velocity_and_acceleration();

        float timestep = settings.min_timestep;

        if (max_velocity > std::numeric_limits<float>::epsilon() && max_acceleration > std::numeric_limits<float>::epsilon()) {
            // try to obtain cfl number
            float max_allowed_timestep = get_non_cfl_validating_timestep(max_acceleration, max_velocity);

            timestep = std::max(timestep, std::min(settings.max_timestep, max_allowed_timestep));
        }

        generated_timestep = timestep;
    }

    void DynamicCflTimestepGenerator::create_compatibility_report(CompatibilityReport& report) {
        initialize();

        report.begin_scope(FLUID_NAMEOF(DynamicCflTimestepGenerator));

        if (parameters.particle_collection == nullptr) {
            report.add_issue("ParticleCollection is null.");
        } else {
            if (!parameters.particle_collection->is_type_present<MovementData>() && !parameters.particle_collection->is_type_present<MovementData3D>()) {
                report.add_issue("Particles are missing the MovementData or MovementData3D attribute.");
            }
            if (!parameters.particle_collection->is_type_present<ParticleInfo>()) {
                report.add_issue("Particles are missing the ParticleInfo attribute.");
            }
        }

        if (parameters.particle_size <= 0.0f) {
            report.add_issue("Particle size is smaller or equal to zero.");
        }
        report.end_scope();
    }
    float DynamicCflTimestepGenerator::get_non_cfl_validating_timestep(float max_acceleration, float max_velocity) {
        FLUID_ASSERT(max_velocity >= 0.0f);
        FLUID_ASSERT(max_acceleration >= 0.0f);
        FLUID_ASSERT(parameters.particle_size > 0.0f);

        FLUID_ASSERT(settings.lambda_a > 0.0f);
        FLUID_ASSERT(settings.lambda_a < 1.0f);

        FLUID_ASSERT(settings.lambda_v > 0.0f);
        FLUID_ASSERT(settings.lambda_v < 1.0f);


        float delta_t_a = settings.max_timestep;
        float delta_t_v = settings.max_timestep;

        if (max_acceleration > 0.0f) {
            delta_t_a = sqrt(parameters.particle_size / max_acceleration) * settings.lambda_a;
        }
        if (max_velocity > 0.0f) {
            delta_t_v = parameters.particle_size / max_velocity * settings.lambda_v;
        }

        auto new_timestep = std::fmin(delta_t_a, delta_t_v);

        return std::fmax(new_timestep, MIN_ALLOWED_TIMESTEP);
    }
    void DynamicCflTimestepGenerator::initialize() {
    }

} // namespace LibFluid
