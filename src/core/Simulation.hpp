#ifndef FLUIDSOLVER_SIMULATION_HPP
#define FLUIDSOLVER_SIMULATION_HPP

#include <core/visualizer/ISimulationVisualizer.hpp>
#include <core/fluidSolver/IISPHFluidSolver.hpp>
#include <core/timestep/ITimestep.hpp>

namespace FluidSolver {
    class Simulation {

    public:
        struct SimulationParameters {
            float particle_size = 1.0f;
            float rest_density = 1.0f;
            float gravity = 9.81f;
            ParticleCollection *collection = nullptr;
            IISPHFluidSolver *fluid_solver = nullptr;
            ISimulationVisualizer *visualizer = nullptr;
            ITimestep *timestep = nullptr;

            bool operator==(const SimulationParameters &other) const;

            bool operator!=(const SimulationParameters &other) const;

        } parameters;


        void execute_simulation_step();

        void visualize();

    private:

        SimulationParameters internal_parameters;

        void initialize();


    };

}

#endif //FLUIDSOLVER_SIMULATION_HPP
