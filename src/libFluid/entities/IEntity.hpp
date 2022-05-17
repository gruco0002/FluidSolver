#pragma once

#include "FluidInclude.hpp"
#include "fluidSolver/ParticleCollection.hpp"
#include "fluidSolver/neighborhoodSearch/NeighborhoodInterface.hpp"
#include "DataChangeStruct.hpp"

#include <memory>

namespace FluidSolver
{
    class IEntity {
      public:
        struct SimulationInformation : public DataChangeStruct
        {

            std::shared_ptr<ParticleCollection> collection = nullptr;
            float gravity = 0.0f;
            float particle_size = 0.0f;
            std::shared_ptr<NeighborhoodInterface> neighborhood_interface = nullptr;

        } sim;

        virtual void execute_simulation_step(pFloat timestep) = 0;

        virtual ~IEntity() = default;
    };
} // namespace FluidSolver
