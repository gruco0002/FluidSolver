#include "ParticleSpawner.hpp"

void FluidSolver::ParticleSpawner::initialize() {
    time_left_over = 0.0f;
}

void FluidSolver::ParticleSpawner::execute_simulation_step(FluidSolver::pFloat timestep) {
    FLUID_ASSERT(collection != nullptr)
    FLUID_ASSERT(collection->is_type_present<ParticleInfo>())
    FLUID_ASSERT(collection->is_type_present<MovementData>())
    FLUID_ASSERT(collection->is_type_present<ParticleData>())
    time_left_over += timestep;

    float spawn_every_second = 1.0f / parameters.particles_per_second;
    pIndex_t next_index = 0;

    while (time_left_over >= spawn_every_second) {
        time_left_over -= spawn_every_second;

        // determine spawn position
        vec2 spawn_position;

        // spawn a particle
        bool spawned = false;
        while (next_index < collection->size()) {
            if (collection->get<ParticleInfo>(next_index).type == ParticleType::ParticleTypeDead) {
                // reuse the current particle
                collection->get<ParticleInfo>(next_index).type = ParticleType::ParticleTypeNormal;
                auto &mv = collection->get<MovementData>(next_index);
                mv.position = spawn_position;
                mv.velocity = parameters.initial_velocity;
                mv.acceleration = vec2(0.0f);
                auto &pd = collection->get<ParticleData>(next_index);
                pd.mass = parameters.mass;
                pd.density = parameters.rest_density;
                pd.pressure = 0.0f;
                spawned = true;
                break;
            }
            next_index++;
        }

        if (!spawned) {
            // we need to add a new particle
            auto index = collection->add();
            auto &pi = collection->get<ParticleInfo>(index);
            pi.type = ParticleType::ParticleTypeNormal;
            pi.tag = index;
            auto &mv = collection->get<MovementData>(index);
            mv.position = spawn_position;
            mv.velocity = parameters.initial_velocity;
            mv.acceleration = vec2(0.0f);
            auto &pd = collection->get<ParticleData>(index);
            pd.mass = parameters.mass;
            pd.density = parameters.rest_density;
            pd.pressure = 0.0f;
        }


    }

}
