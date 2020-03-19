#ifndef FLUIDSOLVER_STATISTICCOLLECTOR_HPP
#define FLUIDSOLVER_STATISTICCOLLECTOR_HPP

#include <cstdint>
#include <core/fluidSolver/particleCollection/IParticleCollection.hpp>
#include <core/selection/IParticleSelection.hpp>
#include <core/selection/AllParticleSelection.hpp>
#include <vector>
#include "StatValue.hpp"
#include <core/fluidSolver/IFluidSolver.hpp>

namespace FluidSolver {

    class StatisticCollector {
    public:

        StatisticCollector();


        float zeroHeight = -500.0f;

        /**
         * The calculated statistics only apply to the selected particles. Particles that are not selected are excluded
         * from the statistics. The results are stored in their respective `calculated...` fields and can be retrieved
         * via their getter functions.
         *
         * @remarks Velocity, CFL number and energies are calculated for particles whose type is normal or boundary.
         */
        virtual void CalculateData();

        virtual ~StatisticCollector();


        const std::vector<StatValue *> &getStats() const;

    protected:
        void RefreshFieldVector();

        StatValue *calculatedAverageDensity = nullptr;
        StatValue *calculatedAverageDensityError = nullptr;
        StatValue *calculatedEnergy = nullptr;
        StatValue *calculatedMaximumVelocity = nullptr;
        StatValue *calculatedDeadParticleCount = nullptr;
        StatValue *calculatedKineticEnergy = nullptr;
        StatValue *calculatedPotentialEnergy = nullptr;
        StatValue *calculatedBoundaryParticleCount = nullptr;
        StatValue *calculatedNormalParticleCount = nullptr;
        StatValue *calculatedCFLNumber = nullptr;
        StatValue *diagonalElement = nullptr;
        StatValue *iterationCount = nullptr;
        StatValue *predictedDensityError = nullptr;
        StatValue *computationTimePerTimestep = nullptr;
        StatValue *pressureSolverComputationTimePerTimestep = nullptr;

        std::vector<StatValue *> Stats;

    private:


        void SetupFields();

        void CleanUpFields();


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

        IFluidSolver *fluidSolver = nullptr;
    public:
        IFluidSolver *getFluidSolver() const;

        void setFluidSolver(IFluidSolver *fluidSolver);


        float getRestDensity() const;

        void setRestDensity(float restDensity);


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
