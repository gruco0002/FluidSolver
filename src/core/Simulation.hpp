#ifndef FLUIDSOLVER_SIMULATION_HPP
#define FLUIDSOLVER_SIMULATION_HPP

#include "core/OutputManager.hpp"
#include "core/entities/IEntity.hpp"
#include "core/fluidSolver/IFluidSolver.hpp"
#include "core/fluidSolver/neighborhoodSearch/NeighborhoodInterface.hpp"
#include "core/sensors/ISensor.hpp"
#include "core/timestep/ITimestep.hpp"
#include "core/visualizer/ISimulationVisualizer.hpp"
#include "core/Compatibility.hpp"

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
        std::shared_ptr<ITimestep> timestep = nullptr;

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


      private:
        Timepoint timepoint;

        SimulationParameters internal_parameters;

        NeighborhoodInterface neigborhood_interface;

        void initialize();
    };

} // namespace FluidSolver

#endif // FLUIDSOLVER_SIMULATION_HPP
