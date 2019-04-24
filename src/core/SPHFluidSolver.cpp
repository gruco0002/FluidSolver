//
// Created by corbi on 24.04.2019.
//

#include <cstdint>
#include "SPHFluidSolver.hpp"

namespace FluidSolver {


    void SPHFluidSolver::ExecuteSimulationStep() {

        // find neighbors for all particles
        if (neighborhoodSearch->SupportsGlobalNeighborhoodFinding()) {
            neighborhoodSearch->FindNeighbors(particleCollection, NeighborhoodRadius);
        } else {
            for (uint32_t i = 0; i < particleCollection->GetSize(); i++) {
                neighborhoodSearch->FindNeighbors(i, particleCollection, NeighborhoodRadius);
            }
        }


        // calculate density and pressure for all particles
        for (uint32_t i = 0; i < particleCollection->GetSize(); i++) {
            float density = ComputeDensity(i);
            particleCollection->SetDensity(i, density);

            float pressure = ComputePressure(i);
            particleCollection->SetPressure(i, pressure);
        }

        // compute non pressure accelerations and pressure accelerations for all particles
        for (uint32_t i = 0; i < particleCollection->GetSize(); i++) {
            glm::vec2 nonPressureAcc = ComputeNonPressureAcceleration(i);
            glm::vec2 pressureAcc = ComputePressureAcceleration(i);
            glm::vec2 acceleration = pressureAcc + nonPressureAcc;
            particleCollection->SetAcceleration(i, acceleration);
        }

        // update velocity and position of all particles
        for (uint32_t i = 0; i < particleCollection->GetSize(); i++) {
            integrationScheme->Integrate(i, particleCollection, TimeStep);
        }

    }

    float SPHFluidSolver::ComputePressure(uint32_t particleIndex) {
        // TODO: implement
        return 0;
    }

    float SPHFluidSolver::ComputeDensity(uint32_t particleIndex) {
        // TODO: implement
        return 0;
    }

    glm::vec2 SPHFluidSolver::ComputeNonPressureAcceleration(uint32_t particleIndex) {
        // TODO: implement
        return glm::vec2(0.0f);
    }

    glm::vec2 SPHFluidSolver::ComputePressureAcceleration(uint32_t particleIndex) {
        // TODO: implement
        return glm::vec2(0.0f);
    }
}