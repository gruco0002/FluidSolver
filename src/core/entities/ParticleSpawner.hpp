#ifndef FLUIDSOLVER_PARTICLESPAWNER_HPP
#define FLUIDSOLVER_PARTICLESPAWNER_HPP

#include "IEntity.hpp"
#include <random>

namespace FluidSolver {
    class ParticleSpawner : public IEntity {
    public:

        struct Parameters {
            struct Area {
                float left;
                float right;

                float bottom;
                float top;
            } area = {};

            float particles_per_second = 0.0f;
            vec2 initial_velocity = vec2(0.0f);

            float mass = 1.0f;
            float rest_density = 1.0f;
        } parameters;

        void initialize() override;

        void execute_simulation_step(pFloat timestep) override;



    private:

        float time_left_over = 0.0f;

        std::mt19937 generator;

    };

}


#endif //FLUIDSOLVER_PARTICLESPAWNER_HPP
