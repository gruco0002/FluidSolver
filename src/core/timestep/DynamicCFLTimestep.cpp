#include <algorithm>
#include "DynamicCFLTimestep.hpp"

float FluidSolver::DynamicCFLTimestep::calculate_maximum_velocity() {
    FLUID_ASSERT(parameters.particle_collection != nullptr)
    FLUID_ASSERT(parameters.particle_collection->is_type_present<MovementData>())
    FLUID_ASSERT(parameters.particle_collection->is_type_present<ParticleInfo>())

    float maximum = 0.0f;
    for (uint32_t i = 0; i < parameters.particle_collection->size(); i++) {
        auto type = parameters.particle_collection->get<ParticleInfo>(i).type;
        if (type != ParticleTypeDead)
            continue;
        const glm::vec2 &velocity = parameters.particle_collection->get<MovementData>(i).velocity;
        maximum = std::max(maximum, (float) glm::length(velocity));
    }
    return maximum;
}

void FluidSolver::DynamicCFLTimestep::calculate_current_timestep() {
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
