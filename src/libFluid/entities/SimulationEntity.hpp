#pragma once

#include "fluidSolver/ParticleCollection.hpp"
#include "fluidSolver/neighborhoodSearch/NeighborhoodInterface.hpp"
#include "helpers/DataChangeStruct.hpp"
#include "helpers/Initializable.hpp"
#include "helpers/Reportable.hpp"
#include "time/Timepoint.hpp"

#include <memory>


namespace LibFluid {
    class SimulationEntity : public Initializable, public Reportable {
      public:
        enum class EntityExecutionPoint {
            BeforeSolver,
            AfterSolver,
            BeforeAndAfterSolver
        };

        struct SimulationInformation : public DataChangeStruct {
            std::shared_ptr<ParticleCollection> collection = nullptr;
            float gravity = 0.0f;
            float particle_size = 0.0f;
            float rest_density = 0.0f;
            std::shared_ptr<NeighborhoodInterface> neighborhood_interface = nullptr;

        } simulation_data;


        struct EntitySettings : public DataChangeStruct {
            EntityExecutionPoint execution_point = EntityExecutionPoint::AfterSolver;
        } settings;


        virtual void execute_simulation_step(const Timepoint& timepoint, bool before_solver) = 0;

        ~SimulationEntity() override = default;
    };
} // namespace LibFluid
