#ifndef FLUIDSOLVER_IISPHFLUIDSOLVER_HPP
#define FLUIDSOLVER_IISPHFLUIDSOLVER_HPP

#include <core/fluidSolver/IFluidSolver.hpp>
#include <core/fluidSolver/kernel/CubicSplineKernel.hpp>
#include <core/fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchDynamicAllocated.hpp>
#include <core/fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch.hpp>

namespace FluidSolver {

    class IISPHFluidSolver : public IFluidSolver<CubicSplineKernel, HashedNeighborhoodSearch> {
    public:
        void initialize() override;

        void execute_simulation_step(pFloat timestep) override;

    private:

        void CalculateDensity(pIndex_t particleIndex);

        void CalculateNonPressureAccelerationAndPredictedVelocity(pIndex_t particleIndex);

        glm::vec2 ComputeViscosityAcceleration(pIndex_t particleIndex);

        void ComputeSourceTerm(pIndex_t particleIndex);

        void ComputeDiagonalElement(pIndex_t particleIndex);

        void InitializePressure(pIndex_t particleIndex);

        void IntegrateParticle(pIndex_t particleIndex);

        void ComputePressure();

        pFloat current_timestep = 0.0f;

    public:

        struct IISPHSettings {
            pFloat MaxDensityErrorAllowed = 0.001f;

            size_t MinNumberOfIterations = 2;
            size_t MaxNumberOfIterations = 100;

            pFloat Omega = 0.5f;
            pFloat Gamma = 0.7f;

            pFloat Viscosity = 5.0f;
        } settings;

        struct IISPHParticleData{
            vec2 predicted_velocity;
            pFloat source_term;
            pFloat diagonal_element;
        };

       static void adapt_collection(ParticleCollection &collection);




    };
}


#endif //FLUIDSOLVER_IISPHFLUIDSOLVER_HPP
