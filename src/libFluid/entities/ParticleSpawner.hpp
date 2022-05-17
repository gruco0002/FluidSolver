#pragma once

#include "IEntity.hpp"

namespace FluidSolver {
    class ParticleSpawner : public IEntity {
      public:
        struct Parameters {
            vec2 position = vec2(0.0f);
            vec2 direction = vec2(0.0f, -1.0f);
            float width = 10.0f;

            float initial_velocity = 0.0f;

            float mass = 1.0f;
            float rest_density = 1.0f;
        } parameters;

        void execute_simulation_step(pFloat timestep) override;


      private:
        float time_left_over = 0.0f;

        pIndex_t last_index_checked = 0;

        pIndex_t get_or_add_particle();

        void spawn_particle(pIndex_t index, const glm::vec2& position, const glm::vec2& initial_velocity);

        bool is_position_free(const glm::vec2& position);
    };

} // namespace FluidSolver
