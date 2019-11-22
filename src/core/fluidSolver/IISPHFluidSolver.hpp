//
// Created by corbi on 20.11.2019.
//

#ifndef FLUIDSOLVER_IISPHFLUIDSOLVER_HPP
#define FLUIDSOLVER_IISPHFLUIDSOLVER_HPP

#include <core/fluidSolver/IFluidSolver.hpp>

#include <core/fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch.hpp>
#include <core/fluidSolver/kernel/CubicSplineKernel.hpp>

namespace FluidSolver {
    class IISPHFluidSolver : public IFluidSolver {

    private:
        float Timestep = 0.0f;
        float RestDensity = 0.0f;
        float ParticleSize = 0.0f;
        float Gravity = 0.0f;

        float KernelSupport = 0.0f;
        float NeighborhoodRadius = 0.0f;

        IParticleCollection* ParticleCollection = nullptr;

        HashedNeighborhoodSearch* neighborhoodSearch = nullptr;
        IKernel* kernel = new CubicSplineKernel();

        void CalculateDensity(uint32_t particleIndex);
        void CalculateNonPressureAccelerationAndPredictedVelocity(uint32_t particleIndex);
        void ComputeSourceTerm(uint32_t particleIndex);
        void ComputeDiagonalElement(uint32_t particleIndex);
        void InitializePressure(uint32_t particleIndex);
        void IntegrateParticle(uint32_t particleIndex);
        void ComputePressure();

    public:

        float MaxDensityErrorAllowed = 0.1f;

        size_t MinNumberOfIterations = 2;

        float Omega = 1.0f;
        float Gamma = 0.7f;

        float getGravity() override;

        void setGravity(float gravity) override;

        void ExecuteSimulationStep() override;

        float getParticleSize() override;

        void setParticleSize(float particleSize) override;

        float getRestDensity() override;

        void setRestDensity(float restDensity) override;

        float getTimestep() override;

        void setTimestep(float timestep) override;

        void setParticleCollection(IParticleCollection *particleCollection) override;

        IParticleCollection *getParticleCollection() override;
    };
}


#endif //FLUIDSOLVER_IISPHFLUIDSOLVER_HPP
