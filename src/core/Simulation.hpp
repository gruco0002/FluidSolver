#ifndef FLUIDSOLVER_SIMULATION_HPP
#define FLUIDSOLVER_SIMULATION_HPP

#include <core/visualizer/ISimulationVisualizer.hpp>
#include <core/fluidSolver/IFluidSolver.hpp>
#include <core/timestep/ITimestep.hpp>
#include <core/entities/IEntity.hpp>
#include <core/sensors/ISensor.hpp>

namespace FluidSolver {
    struct SimulationParameters {
        float particle_size = 1.0f;
        float rest_density = 1.0f;
        float gravity = 9.81f;
        ParticleCollection *collection = nullptr;
        IFluidSolverBase *fluid_solver = nullptr;
        ISimulationVisualizer *visualizer = nullptr;
        ITimestep *timestep = nullptr;

        std::vector<IEntity *> entities;

        SensorDataStorage *sensor_storage = nullptr;
        std::vector<ISensor *> sensors;

        bool invalidate = false;

        bool operator==(const SimulationParameters &other) const;

        bool operator!=(const SimulationParameters &other) const;

    };

    class Simulation {

    public:
        SimulationParameters parameters;

        void execute_simulation_step();

        void visualize(bool update_data = false);

    private:

        SimulationParameters internal_parameters;

        void initialize();


    };

}

#endif //FLUIDSOLVER_SIMULATION_HPP
