//
// Created by corbi on 24.04.2019.
//

#ifndef FLUIDSOLVER_SPHFLUIDSOLVER_HPP
#define FLUIDSOLVER_SPHFLUIDSOLVER_HPP

#include <core/interface/IIntegrationScheme.hpp>
#include <core/interface/INeighborhoodSearch.hpp>
#include <core/interface/IKernel.hpp>
#include "core/interface/IParticleCollection.hpp"

namespace FluidSolver {
    class SPHFluidSolver {


    public:

        float TimeStep = 0.01f;
        float ParticleSize = 1.0f;
        float NeighborhoodRadius = 5.0f;
        float KernelSupport = 2.0f;
        float RestDensity = 1.0f;
        float StiffnessK = 0.001f;
        float Viscosity = 0.01f;

        float Gravity = 9.81f;

        IParticleCollection *particleCollection = nullptr;
        IIntegrationScheme *integrationScheme = nullptr;
        INeighborhoodSearch *neighborhoodSearch = nullptr;
        IKernel *kernel = nullptr;

        void ExecuteSimulationStep();

        virtual float ComputePressure(uint32_t particleIndex);

        virtual float ComputeDensity(uint32_t particleIndex);

        virtual glm::vec2 ComputeNonPressureAcceleration(uint32_t particleIndex);

        virtual glm::vec2 ComputePressureAcceleration(uint32_t particleIndex);

        virtual glm::vec2 ComputeViscosityAcceleration(uint32_t particleIndex);


    };
}


#endif //FLUIDSOLVER_SPHFLUIDSOLVER_HPP
