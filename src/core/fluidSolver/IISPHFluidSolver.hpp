#ifndef FLUIDSOLVER_IISPHFLUIDSOLVER_HPP
#define FLUIDSOLVER_IISPHFLUIDSOLVER_HPP

#include <core/fluidSolver/IFluidSolver.hpp>
#include <core/fluidSolver/kernel/CubicSplineKernel.hpp>
#include <core/fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchDynamicAllocated.hpp>

namespace FluidSolver {

    class IISPHFluidSolver : public IFluidSolver<CubicSplineKernel, QuadraticNeighborhoodSearchDynamicAllocated> {
    public:
        void execute_simulation_step(float timestep) override;

    private:

        void CalculateDensity(uint32_t particleIndex);

        void CalculateNonPressureAccelerationAndPredictedVelocity(uint32_t particleIndex);

        glm::vec2 ComputeViscosityAcceleration(uint32_t particleIndex);

        void ComputeSourceTerm(uint32_t particleIndex);

        void ComputeDiagonalElement(uint32_t particleIndex);

        void InitializePressure(uint32_t particleIndex);

        void IntegrateParticle(uint32_t particleIndex);

        void ComputePressure();

        float current_timestep = 0.0f;

    public:

        struct IISPHSettings {
            float MaxDensityErrorAllowed = 0.001f;

            size_t MinNumberOfIterations = 2;
            size_t MaxNumberOfIterations = 100;

            float Omega = 0.5f;
            float Gamma = 0.7f;

            float Viscosity = 5.0f;
        } settings;

        struct IISPHParticleData{
            glm::vec2 predicted_velocity;
            float source_term;
            float diagonal_element;
        };

        void adapt_collection(ParticleCollection &collection);




    };
}


#endif //FLUIDSOLVER_IISPHFLUIDSOLVER_HPP
