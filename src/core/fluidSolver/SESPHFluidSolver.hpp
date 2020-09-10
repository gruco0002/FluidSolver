#ifndef FLUIDSOLVER_SESPHFLUIDSOLVER_HPP
#define FLUIDSOLVER_SESPHFLUIDSOLVER_HPP


#include <core/fluidSolver/IFluidSolver.hpp>
#include <core/fluidSolver/kernel/CubicSplineKernel.hpp>
#include <core/fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchDynamicAllocated.hpp>


namespace FluidSolver {
    class SESPHFluidSolver : public IFluidSolver<CubicSplineKernel, QuadraticNeighborhoodSearchDynamicAllocated> {

    public:
        void execute_simulation_step(float timestep) override;

    private:

        float current_timestep = 0.0f;

        float ComputePressure(uint32_t particleIndex);

        float ComputeDensity(uint32_t particleIndex);

        glm::vec2 ComputeNonPressureAcceleration(uint32_t particleIndex);

        glm::vec2 ComputePressureAcceleration(uint32_t particleIndex);

        glm::vec2 ComputeViscosityAcceleration(uint32_t particleIndex);


    public:
        struct SESPHSettings {
            float StiffnessK = 100000.0f;
            float Viscosity = 5.0f;
        } settings;


    };
}


#endif //FLUIDSOLVER_SESPHFLUIDSOLVER_HPP
