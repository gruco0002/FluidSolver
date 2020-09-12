#ifndef FLUIDSOLVER_SESPHFLUIDSOLVER_HPP
#define FLUIDSOLVER_SESPHFLUIDSOLVER_HPP


#include <core/fluidSolver/IFluidSolver.hpp>
#include <core/fluidSolver/kernel/CubicSplineKernel.hpp>
#include <core/fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchDynamicAllocated.hpp>


namespace FluidSolver {
    class SESPHFluidSolver : public IFluidSolver<CubicSplineKernel, QuadraticNeighborhoodSearchDynamicAllocated> {

    public:
        void execute_simulation_step(pFloat timestep) override;

    private:
    public:
        void initialize() override;

    private:
        pFloat current_timestep = 0.0f;

        pFloat ComputePressure(pIndex_t particleIndex);

        pFloat ComputeDensity(pIndex_t particleIndex);

        vec2 ComputeNonPressureAcceleration(pIndex_t particleIndex);

        vec2 ComputePressureAcceleration(pIndex_t particleIndex);

        vec2 ComputeViscosityAcceleration(pIndex_t particleIndex);


    public:
        struct SESPHSettings {
            pFloat StiffnessK = 100000.0f;
            pFloat Viscosity = 5.0f;
        } settings;


    };
}


#endif //FLUIDSOLVER_SESPHFLUIDSOLVER_HPP
