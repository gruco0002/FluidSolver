//
// Created by corbi on 24.04.2019.
//

#ifndef FLUIDSOLVER_SPHFLUIDSOLVER_HPP
#define FLUIDSOLVER_SPHFLUIDSOLVER_HPP

#include <core/fluidSolver/neighborhoodSearch/INeighborhoodSearch.hpp>
#include <core/fluidSolver/kernel/IKernel.hpp>
#include <core/interface/ISimulationModifier.hpp>
#include <core/fluidSolver/IFluidSolver.hpp>
#include "core/fluidSolver/particleCollection/IParticleCollection.hpp"
#include "core/StatisticCollector.hpp"

namespace FluidSolver {
    class SPHFluidSolver : public IFluidSolver {

    private:
        float Gravity = 9.81f;
        float TimeStep = 0.001f;
        float ParticleSize = 1.0f;
        float RestDensity = 1.0f;

        IParticleCollection *particleCollection = nullptr;

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


        float NeighborhoodRadius = 5.0f;
        float KernelSupport = 2.0f;

        float StiffnessK = 0.001f;
        float Viscosity = 0.01f;

        INeighborhoodSearch *neighborhoodSearch = nullptr;
        IKernel *kernel = nullptr;

        void ExecuteSimulationStep() override;

        float ComputePressure(uint32_t particleIndex);

        float ComputeDensity(uint32_t particleIndex);

        glm::vec2 ComputeNonPressureAcceleration(uint32_t particleIndex);

        glm::vec2 ComputePressureAcceleration(uint32_t particleIndex);

        glm::vec2 ComputeViscosityAcceleration(uint32_t particleIndex);

        virtual ~SPHFluidSolver();
    };
}


#endif //FLUIDSOLVER_SPHFLUIDSOLVER_HPP
