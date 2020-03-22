#ifndef FLUIDSOLVER_SESPHFLUIDSOLVER_HPP
#define FLUIDSOLVER_SESPHFLUIDSOLVER_HPP

#include <core/fluidSolver/neighborhoodSearch/INeighborhoodSearch.hpp>
#include <core/fluidSolver/kernel/IKernel.hpp>
#include <core/interface/ISimulationModifier.hpp>
#include <core/fluidSolver/IFluidSolver.hpp>
#include <core/fluidSolver/kernel/CubicSplineKernel.hpp>
#include "core/fluidSolver/particleCollection/IParticleCollection.hpp"
#include "core/statistics/StatisticCollector.hpp"

namespace FluidSolver {
    class SESPHFluidSolver : public IFluidSolver {

    private:
        float Gravity = 9.81f;
    public:
        uint32_t GetComputationTimeLastTimestepInMicroseconds() override;

        uint32_t GetComputationTimePressureSolverLastTimestepInMicroseconds() override;

    private:
        float TimeStep = 0.001f;
        float ParticleSize = 1.0f;
        float RestDensity = 1.0f;


        float NeighborhoodRadius = 5.0f;
        float KernelSupport = 2.0f;

        uint32_t compTimeTotalMicroseconds = 0;
        uint32_t compTimePressureSolverMicroseconds = 0;

        IParticleCollection *particleCollection = nullptr;

        INeighborhoodSearch *neighborhoodSearch = nullptr;
        IKernel *kernel = new FluidSolver::CubicSplineKernel();

        float ComputePressure(uint32_t particleIndex);

        float ComputeDensity(uint32_t particleIndex);

        glm::vec2 ComputeNonPressureAcceleration(uint32_t particleIndex);

        glm::vec2 ComputePressureAcceleration(uint32_t particleIndex);

        glm::vec2 ComputeViscosityAcceleration(uint32_t particleIndex);

    public:
        SESPHFluidSolver() = default;


    public:
        float getGravity() override;

        void setGravity(float gravity) override;

        float getParticleSize() override;

        void setParticleSize(float particleSize) override;

        float getRestDensity() override;

        void setRestDensity(float restDensity) override;

        float getTimestep() override;

        void setTimestep(float timestep) override;

        void setParticleCollection(IParticleCollection *particleCollection) override;

        IParticleCollection *getParticleCollection() override;


        float StiffnessK = 100000.0f;
        float Viscosity = 5.0f;


        void ExecuteSimulationStep() override;

        ~SESPHFluidSolver() override;
    };
}


#endif //FLUIDSOLVER_SESPHFLUIDSOLVER_HPP
