#ifndef FLUIDSOLVER_IISPHFLUIDSOLVER_HPP
#define FLUIDSOLVER_IISPHFLUIDSOLVER_HPP

#include <core/fluidSolver/IFluidSolver.hpp>
#include <core/fluidSolver/kernel/IKernel.hpp>

namespace FluidSolver {
    class IISPHFluidSolver : public IFluidSolver {

    public:
        uint32_t GetComputationTimeLastTimestepInMicroseconds() override;

        uint32_t GetComputationTimePressureSolverLastTimestepInMicroseconds() override;

    private:
        float Timestep = 0.0f;
        float RestDensity = 0.0f;
        float ParticleSize = 0.0f;
        float Gravity = 0.0f;

        float lastPredictedDensityError = 0.0f;
        float maxPredictedDensityErrorReached = 0.0f;

        uint32_t compTimeTotalMicroseconds = 0;
        uint32_t compTimePressureSolverMicroseconds = 0;

    public:
        float getMaxPredictedDensityErrorReached() const;

    private:
        size_t lastIterationCount = 0;
    public:
        float getLastPredictedDensityError() const;

        size_t getLastIterationCount() const;

    private:

        IParticleCollection *ParticleCollection = nullptr;

        INeighborhoodSearch *neighborhoodSearch = nullptr;
        IKernel *kernel = nullptr;

        void CalculateDensity(uint32_t particleIndex);

        void CalculateNonPressureAccelerationAndPredictedVelocity(uint32_t particleIndex);

        glm::vec2 ComputeViscosityAcceleration(uint32_t particleIndex);

        void ComputeSourceTerm(uint32_t particleIndex);

        void ComputeDiagonalElement(uint32_t particleIndex);

        void InitializePressure(uint32_t particleIndex);

        void IntegrateParticle(uint32_t particleIndex);

        void ComputePressure();

    public:

        float MaxDensityErrorAllowed = 0.001f;

        size_t MinNumberOfIterations = 2;
        size_t MaxNumberOfIterations = 100;

        float Omega = 0.5f;
        float Gamma = 0.7f;
        float Viscosity = 5.0f;

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

        void SetKernel(IKernel *kernel) override;

        IKernel *GetKernel() override;

        void SetNeighborhoodSearch(INeighborhoodSearch *neighborhoodSearch) override;

        INeighborhoodSearch *GetNeighborhoodSearch() override;
    };
}


#endif //FLUIDSOLVER_IISPHFLUIDSOLVER_HPP
