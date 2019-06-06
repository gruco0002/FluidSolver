//
// Created by corbi on 06.06.2019.
//

#ifndef FLUIDSOLVER_SPAWNAREA_HPP
#define FLUIDSOLVER_SPAWNAREA_HPP

#include <core/interface/ISimulationModifier.hpp>
#include <random>


namespace FluidSolver {
    /**
     * Revives dead particles and spawns them in the simulation.
     */
    class SpawnArea : public ISimulationModifier {

    public:

        SpawnArea(float top, float left, float bottom, float right, float spawnRate, const glm::vec2 initialVelocity);

        /**
         * Spawnrate in particles per second.
         */
        float spawnRate;

        // spawn area
        float top;
        float left;
        float bottom;
        float right;

        glm::vec2 initialVelocity;

        void ModifySimulation(IParticleCollection *collection, SimulationInfo &info) override;

    private:
        float timeAccumulator = 0.0f;

        glm::vec2 GenerateRandomPosition();

        std::minstd_rand pseudoRandomGenerator;


    };
}


#endif //FLUIDSOLVER_SPAWNAREA_HPP
