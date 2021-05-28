#ifndef FLUIDSOLVER_SIMULATION_HPP
#define FLUIDSOLVER_SIMULATION_HPP

#include "core/OutputManager.hpp"
#include "core/entities/IEntity.hpp"
#include "core/fluidSolver/IFluidSolver.hpp"
#include "core/fluidSolver/neighborhoodSearch/NeighborhoodInterface.hpp"
#include "core/sensors/ISensor.hpp"
#include "core/timestep/ITimestep.hpp"
#include "core/visualizer/ISimulationVisualizer.hpp"

namespace FluidSolver
{
    struct SimulationParameters
    {
        float particle_size = 1.0f;
        float rest_density = 1.0f;
        float gravity = 9.81f;
        ParticleCollection* collection = nullptr;
        IFluidSolverBase* fluid_solver = nullptr;
        ISimulationVisualizer* visualizer = nullptr;
        ITimestep* timestep = nullptr;

        std::vector<IEntity*> entities;

        std::vector<ISensor*> sensors;

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


      private:
        Timepoint timepoint;

        SimulationParameters internal_parameters;

        NeighborhoodInterface neigborhood_interface;

        void initialize();
    };

} // namespace FluidSolver

#endif // FLUIDSOLVER_SIMULATION_HPP
