//
// Created by corbi on 24.04.2019.
//

#ifndef FLUIDSOLVER_SPHFLUIDSOLVER_HPP
#define FLUIDSOLVER_SPHFLUIDSOLVER_HPP

#include <core/interface/IIntegrationScheme.hpp>
#include <core/interface/INeighborhoodSearch.hpp>
#include <core/interface/IKernel.hpp>
#include <core/interface/ISimulationModifier.hpp>
#include <core/interface/IFluidSolver.hpp>
#include "core/interface/IParticleCollection.hpp"
#include "StatisticCollector.hpp"

namespace FluidSolver {
    class SPHFluidSolver : public IFluidSolver {


    public:
        float getParticleSize() override;

        void setParticleSize(float particleSize) override;

        float getRestDensity() override;

        void setRestDensity(float restDensity) override;

        float getTimestep() override;

        void setTimestep(float timestep) override;

        void setParticleCollection(IParticleCollection *particleCollection) override;

        IParticleCollection *getParticleCollection() override;

        float TimeStep = 0.001f;
        float ParticleSize = 1.0f;
        float NeighborhoodRadius = 5.0f;
        float KernelSupport = 2.0f;
        float RestDensity = 1.0f;
        float StiffnessK = 0.001f;
        float Viscosity = 0.01f;

        float Gravity = 9.81f;

        IParticleCollection *particleCollection = nullptr;
        INeighborhoodSearch *neighborhoodSearch = nullptr;
        IKernel *kernel = nullptr;

        std::vector<ISimulationModifier *> simulationModifiers;

        void ExecuteSimulationStep() override;

        float ComputePressure(uint32_t particleIndex);

        float ComputeDensity(uint32_t particleIndex);

        glm::vec2 ComputeNonPressureAcceleration(uint32_t particleIndex);

        glm::vec2 ComputePressureAcceleration(uint32_t particleIndex);

        glm::vec2 ComputeViscosityAcceleration(uint32_t particleIndex);

        StatisticCollector *statisticCollector = new StatisticCollector(this);

        virtual ~SPHFluidSolver();
    };
}


#endif //FLUIDSOLVER_SPHFLUIDSOLVER_HPP
