#include "ParticleSpawner.hpp"

namespace LibFluid {
    void ParticleSpawner::execute_simulation_step(const Timepoint& timepoint, bool before_solver) {
        initialize();


        FLUID_ASSERT(simulation_data.collection != nullptr)
        FLUID_ASSERT(simulation_data.collection->is_type_present<ParticleInfo>())
        FLUID_ASSERT(simulation_data.collection->is_type_present<MovementData>())
        FLUID_ASSERT(simulation_data.collection->is_type_present<ParticleData>())

        FLUID_ASSERT(simulation_data.particle_size > 0.0f)

        FLUID_ASSERT(parameters.direction != glm::vec2(0.0f))
        FLUID_ASSERT(parameters.width > 0.0f)
        FLUID_ASSERT(parameters.initial_velocity > 0.0f)

        // reset stuff
        this->last_index_checked = 0;

        // add time
        time_left_over += timepoint.actual_time_step;


        // assume that the particles move away by their velocity
        if (time_left_over * parameters.initial_velocity >= simulation_data.particle_size) {
            // there should be enough place to spawn the particles

            // calculated required data
            glm::vec2 normalized_direction = glm::normalize(parameters.direction);
            glm::vec2 initial_velocity = normalized_direction * parameters.initial_velocity;
            glm::vec2 orth_direction = glm::vec2(normalized_direction.y, -normalized_direction.x);
            float width = parameters.width;

            // spawn particles along width
            for (float x = -width / 2.0f; x <= width / 2.0f; x += simulation_data.particle_size) {
                glm::vec2 pos = parameters.position + orth_direction * x +
                        (time_left_over * parameters.initial_velocity - simulation_data.particle_size) * normalized_direction;

                // check if the position is free (avoid spawning particles inside each other, as this would represent
                // invalid states)
                if (!is_position_free(pos))
                    continue;

                auto index = get_or_add_particle();
                spawn_particle(index, pos, initial_velocity);
            }

            // we can set it to zero, since we already included the offset of a larger time_left_over in the position
            // calculation
            time_left_over = 0.0f;
        }
    }

    size_t ParticleSpawner::get_or_add_particle() {
        while (last_index_checked < simulation_data.collection->size()) {
            if (simulation_data.collection->get<ParticleInfo>(last_index_checked).type == ParticleType::ParticleTypeInactive) {
                // reuse the current particle
                return last_index_checked;
            }
            last_index_checked++;
        }

        // could not find an inactive particle --> add a new one
        auto index = simulation_data.collection->add();
        return index;
    }

    void ParticleSpawner::spawn_particle(size_t index, const glm::vec2& position,
            const glm::vec2& initial_velocity) {
        FLUID_ASSERT(index < simulation_data.collection->size());

        // set the particle type to normal
        simulation_data.collection->get<ParticleInfo>(index).type = ParticleType::ParticleTypeNormal;
        ;

        // set default particle data
        auto& pd = simulation_data.collection->get<ParticleData>(index);
        pd.density = parameters.rest_density;
        pd.mass = parameters.mass;
        pd.pressure = 0.0f;

        // set movement data and position
        auto& md = simulation_data.collection->get<MovementData>(index);
        md.position = position;
        md.acceleration = glm::vec2(0.0f);
        md.velocity = initial_velocity;
    }

    bool ParticleSpawner::is_position_free(const glm::vec2& position) {
        FLUID_ASSERT(simulation_data.neighborhood_interface != nullptr);
        FLUID_ASSERT(simulation_data.collection != nullptr);

        const float epsilon = 0.95f;

        float min_distance_squared = (simulation_data.particle_size * epsilon) * (simulation_data.particle_size * epsilon);

        auto neighbors = simulation_data.neighborhood_interface->get_neighbors(position);
        for (auto& neighbor : neighbors) {
            auto& neighbor_pos = simulation_data.collection->get<MovementData>(neighbor).position;
            glm::vec2 diff = neighbor_pos - position;
            float distance_squared = glm::dot(diff, diff);
            if (distance_squared < min_distance_squared) {
                return false;
            }
        }

        return true;
    }
    void ParticleSpawner::initialize() {
        if (simulation_data.has_data_changed()) {
            simulation_data.acknowledge_data_change();

            time_left_over = 0.0f;
        }

        if (settings.has_data_changed()) {
            settings.acknowledge_data_change();
        }
    }
    void ParticleSpawner::create_compatibility_report(CompatibilityReport& report) {
        initialize();
    }
} // namespace LibFluid