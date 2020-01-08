//
// Created by corbi on 12.06.2019.
//

#ifndef FLUIDSOLVER_STATISTICCOLLECTOR_HPP
#define FLUIDSOLVER_STATISTICCOLLECTOR_HPP

#include <cstdint>
#include <core/fluidSolver/particleCollection/IParticleCollection.hpp>
#include <core/selection/IParticleSelection.hpp>
#include <core/selection/AllParticleSelection.hpp>

namespace FluidSolver {

    class StatisticCollector {
    public:

        StatisticCollector() = default;


        float zeroHeight = -500.0f;

        /**
         * The calculated statistics only apply to the selected particles. Particles that are not selected are excluded
         * from the statistics. The results are stored in their respective `calculated...` fields and can be retrieved
         * via their getter functions.
         *
         * @remarks Density is only considered for particles of normal type whose density is larger or equal to the rest
         *          density.
         * @remarks Velocity, CFL number and energies are calculated for particles whose type is normal or boundary.
         */
        virtual void CalculateData();

        virtual ~StatisticCollector();

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

        IParticleSelection *particleSelection = nullptr;
    public:
        IParticleSelection *getParticleSelection() const;

        void setParticleSelection(IParticleSelection *particleSelection);

    private:


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
