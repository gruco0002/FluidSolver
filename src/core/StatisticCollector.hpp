//
// Created by corbi on 12.06.2019.
//

#ifndef FLUIDSOLVER_STATISTICCOLLECTOR_HPP
#define FLUIDSOLVER_STATISTICCOLLECTOR_HPP

#include <cstdint>
#include <core/fluidSolver/particleCollection/IParticleCollection.hpp>

namespace FluidSolver {

    class StatisticCollector {
    public:

        StatisticCollector();


        float zeroHeight = -500.0f;

        void CalculateData();

    private:

        float calculatedAverageDensity = 0.0f;
        float calculatedEnergy = 0.0f;
        float calculatedMaximumVelocity = 0.0f;
        uint32_t calculatedDeadParticleCount = 0;
        float calculatedKineticEnergy = 0.0f;
        float calculatedPotentialEnergy = 0.0f;
        uint32_t calculatedBoundaryParticleCount = 0;
        uint32_t calculatedNormalParticleCount = 0;
        float calculatedCFLNumber = 0.0f;
    public:
        float getCalculatedAverageDensity() const;

        float getCalculatedEnergy() const;

        float getCalculatedMaximumVelocity() const;

        uint32_t getCalculatedDeadParticleCount() const;

        float getCalculatedKineticEnergy() const;

        float getCalculatedPotentialEnergy() const;

        uint32_t getCalculatedBoundaryParticleCount() const;

        uint32_t getCalculatedNormalParticleCount() const;

        float getCalculatedCflNumber() const;

    private:


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

        uint32_t GetNormalParticleCount();

        float GetCFLNumber();

        float GetCFLNumber(float maximumVelocity);


        IParticleCollection *particleCollection = nullptr;

        float Timestep = 0.0f;
        float ParticleSize = 1.0f;
        float Gravity = 0.0f;
        float RestDensity = 1.0f;


    public:
        float getRestDensity() const;

        void setRestDensity(float restDensity);

    public:
        IParticleCollection *getParticleCollection() const;

        void setParticleCollection(IParticleCollection *particleCollection);

        float getTimestep() const;

        void setTimestep(float timestep);

        float getParticleSize() const;

        void setParticleSize(float particleSize);

        float getGravity() const;

        void setGravity(float gravity);

    };
}

#endif //FLUIDSOLVER_STATISTICCOLLECTOR_HPP
