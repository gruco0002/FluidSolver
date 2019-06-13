//
// Created by corbi on 12.06.2019.
//

#ifndef FLUIDSOLVER_STATISTICCOLLECTOR_HPP
#define FLUIDSOLVER_STATISTICCOLLECTOR_HPP

#include <cstdint>

namespace FluidSolver{

    class SPHFluidSolver;

class StatisticCollector {
public:

    StatisticCollector(SPHFluidSolver *sphFluidSolver);

    SPHFluidSolver* sphFluidSolver;

    float zeroHeight = -500.0f;

    /**
     * Calculates the average density. Boundary Particles and Particles with Density Below the Rest Density are ignored.
     * @return
     */
     float CalculateAverageDensity();

    /**
     * Calculates the current Energy of all particles except Boundary Particles.
     * @param zeroHeight The zero Height used for potential energy calculation.
     * @param gravity Used for potential energy calculation.
     * @return
     */
     float CalculateEnergy();

     /**
      * Calculates the total Energy provided precalculated information.
      * Result is the same as in calculate energy, but calculated way faster.
      * @param kineticEnergy
      * @param potentialEnergy
      * @return
      */
     float CalculateEnergy(float kineticEnergy, float potentialEnergy);

    /**
     * Calculates the maximal velocity of all particles.
     * @return
     */
     float CalculateMaximumVelocity();

    /**
     * Return the number of dead particles.
     * @return
     */
     uint32_t GetDeadParticleCount();

     float CalculateKineticEnergy();

     float CalculatePotentialEnergy();

     uint32_t GetBoundaryParticleCount();

     float GetCFLNumber();

     float GetCFLNumber(float maximumVelocity);

};
}

#endif //FLUIDSOLVER_STATISTICCOLLECTOR_HPP
