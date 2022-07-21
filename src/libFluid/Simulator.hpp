#pragma once

#include "CompatibilityReport.hpp"
#include "DataChangeStruct.hpp"
#include "OutputManager.hpp"
#include "entities/IEntity.hpp"
#include "fluidSolver/IFluidSolver.hpp"
#include "fluidSolver/neighborhoodSearch/NeighborhoodInterface.hpp"
#include "sensors/ISensor.hpp"
#include "time/ITimestepGenerator.hpp"

#include <memory>


namespace LibFluid {

    class Simulator : public Initializable, public Reportable {
      public:
        struct SimulatorData : public DataChangeStruct {
            std::shared_ptr<ParticleCollection> collection = nullptr;
            std::shared_ptr<IFluidSolverBase> fluid_solver = nullptr;
            std::shared_ptr<ITimestepGenerator> timestep_generator = nullptr;

            std::vector<std::shared_ptr<IEntity>> entities;
            std::vector<std::shared_ptr<ISensor>> sensors;
        } data;


        struct SimulatorParameters : public DataChangeStruct {
            float particle_size = 1.0f;
            float rest_density = 1.0f;
            float gravity = 9.81f;
        } parameters;


        const Timepoint& get_current_timepoint() const;

        void set_timepoint(const Timepoint& timepoint);

        std::shared_ptr<OutputManager> output = nullptr;

      private:
        Timepoint timepoint;

        std::shared_ptr<NeighborhoodInterface> neigborhood_interface = nullptr;


      public:
        Simulator();

        void execute_simulation_step();

        // TODO: rename in "force_initialization_in_next_execution_step"
        void manual_initialize();

        void initialize() override;

        void create_compatibility_report(CompatibilityReport& report) override;

        std::shared_ptr<NeighborhoodInterface> get_neighborhood_interface();
    };

} // namespace FluidSolver
