#pragma once

#include "Compatibility.hpp"
#include "OutputManager.hpp"
#include "entities/IEntity.hpp"
#include "fluidSolver/IFluidSolver.hpp"
#include "fluidSolver/neighborhoodSearch/NeighborhoodInterface.hpp"
#include "sensors/ISensor.hpp"
#include "timestep/ITimestepGenerator.hpp"
#include "visualizer/ISimulationVisualizer.hpp"

#include <memory>


namespace FluidSolver
{
    struct SimulationParameters
    {
        float particle_size = 1.0f;
        float rest_density = 1.0f;
        float gravity = 9.81f;
        std::shared_ptr<ParticleCollection> collection = nullptr;
        std::shared_ptr<IFluidSolverBase> fluid_solver = nullptr;
        std::shared_ptr<ISimulationVisualizer> visualizer = nullptr;
        std::shared_ptr<ITimestepGenerator> timestep = nullptr;

        std::vector<std::shared_ptr<IEntity>> entities;

        std::vector<std::shared_ptr<ISensor>> sensors;

        OutputManager output;

        bool invalidate = false;

        bool operator==(const SimulationParameters& other) const;

        bool operator!=(const SimulationParameters& other) const;
    };

    class Simulation {

      public:
        SimulationParameters parameters;

        void execute_simulation_step();

        void visualize();

        void manual_initialize();

        void check_for_initialization();

        Compatibility check();

        const Timepoint& get_current_timepoint() const;

      private:
        Timepoint timepoint;

        SimulationParameters internal_parameters;

        NeighborhoodInterface neigborhood_interface;

        void initialize();
    };

} // namespace FluidSolver
