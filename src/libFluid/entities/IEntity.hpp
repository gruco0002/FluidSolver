#ifndef FLUIDSOLVER_IENTITY_HPP
#define FLUIDSOLVER_IENTITY_HPP

#include "FluidInclude.hpp"
#include "fluidSolver/ParticleCollection.hpp"
#include "fluidSolver/neighborhoodSearch/NeighborhoodInterface.hpp"

#include <memory>

namespace FluidSolver
{
    class IEntity {
      public:
        struct SimulationInformation
        {

            std::shared_ptr<ParticleCollection> collection = nullptr;
            float gravity = 0.0f;
            float particle_size = 0.0f;
            NeighborhoodInterface* neighborhood_interface = nullptr;

        } sim;


        virtual void initialize() = 0;

        virtual void execute_simulation_step(pFloat timestep) = 0;

        virtual ~IEntity() = default;
    };
} // namespace FluidSolver


#endif // FLUIDSOLVER_IENTITY_HPP