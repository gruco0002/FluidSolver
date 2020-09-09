#ifndef FLUIDSOLVER_IFLUIDSOLVER_HPP
#define FLUIDSOLVER_IFLUIDSOLVER_HPP

#include "ParticleCollection.hpp"
#include "FluidAssert.hpp"

namespace FluidSolver {

    template <typename Kernel, typename NeighborhoodSearch>
    class IFluidSolver {
    public:
        struct SimulationParameters{
            float rest_density = 1.0f;
            float gravity = 9.81f;
            float particle_size = 1.0f;
        } parameters;

        Kernel kernel;

        NeighborhoodSearch neighborhood_search;

        ParticleCollection *collection = nullptr;

        virtual void execute_simulation_step(float timestep) = 0;

        virtual ~IFluidSolver() = default;

    };


}
#endif //FLUIDSOLVER_IFLUIDSOLVER_HPP
