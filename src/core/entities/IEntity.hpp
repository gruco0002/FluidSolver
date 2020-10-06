#ifndef FLUIDSOLVER_IENTITY_HPP
#define FLUIDSOLVER_IENTITY_HPP

#include "../FluidInclude.hpp"
#include "../fluidSolver/ParticleCollection.hpp"

namespace FluidSolver {
    class IEntity {
    public:

        ParticleCollection *collection;

        virtual void initialize() = 0;

        virtual void execute_simulation_step(pFloat timestep) = 0;

        virtual ~IEntity() = default;

    };
}


#endif //FLUIDSOLVER_IENTITY_HPP
