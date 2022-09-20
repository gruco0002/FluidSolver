#pragma once

#include "DataChangeStruct.hpp"
#include "FluidInclude.hpp"
#include "fluidSolver/ParticleCollection.hpp"
#include "fluidSolver/neighborhoodSearch/NeighborhoodInterface.hpp"

#include "Initializable.hpp"
#include "Reportable.hpp"
#include <memory>



namespace LibFluid {
    class SimulationEntity : public Initializable, public Reportable {
      public:
        struct SimulationInformation : public DataChangeStruct {
            std::shared_ptr<ParticleCollection> collection = nullptr;
            float gravity = 0.0f;
            float particle_size = 0.0f;
            std::shared_ptr<NeighborhoodInterface> neighborhood_interface = nullptr;

        } simulation_data;

    
        virtual void execute_simulation_step(pFloat timestep) = 0;

        ~SimulationEntity() override = default;
    };
} // namespace LibFluid
