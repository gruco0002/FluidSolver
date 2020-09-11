#include "ParticleStatistics.hpp"
#include "core/FluidAssert.hpp"
#include <algorithm>


FluidSolver::ParticleStatistics
FluidSolver::ParticleStatistics::fill_data(FluidSolver::ParticleCollection *collection) {
    FLUID_ASSERT(collection->is_type_present<MovementData>());
    FLUID_ASSERT(collection->is_type_present<ParticleData>());
    FLUID_ASSERT(collection->is_type_present<ParticleInfo>());


    ParticleStatistics s = {};
    size_t counter = 0;

    for (size_t i = 0; i < collection->size(); i++) {
        const MovementData &mData = collection->get<MovementData>(i);
        const ParticleData &pData = collection->get<ParticleData>(i);
        const ParticleInfo &iData = collection->get<ParticleInfo>(i);

        if (iData.type == ParticleType::ParticleTypeNormal) {
            s.normal_particles++;

            s.average_density += pData.density;
            s.min_density = std::min(s.min_density, pData.density);
            s.max_density = std::max(s.max_density, pData.density);

            s.average_pressure += pData.pressure;
            s.min_pressure = std::min(s.min_pressure, pData.pressure);
            s.max_pressure = std::max(s.max_pressure, pData.pressure);

            float velocity = glm::length(mData.velocity);
            s.average_velocity += glm::length(velocity);
            s.min_velocity = std::min(s.min_velocity, velocity);
            s.max_velocity = std::max(s.max_velocity, velocity);

            s.kinetic_energy += 0.5f * pData.mass * velocity * velocity;
            s.potential_energy += pData.mass * mData.position.y * 9.81f;

            counter++;
        } else if (iData.type == ParticleType::ParticleTypeBoundary) {
            s.boundary_particles++;
        } else if (iData.type == ParticleType::ParticleTypeDead) {
            s.inactive_particles++;
        }


    }

    if (counter > 0) {
        s.average_pressure /= counter;
        s.average_velocity /= counter;
        s.average_density /= counter;
    }

    return s;
}
