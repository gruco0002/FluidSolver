#ifndef FLUIDSOLVER_ISENSOR_HPP
#define FLUIDSOLVER_ISENSOR_HPP


#include "SensorDataStorage.hpp"

namespace FluidSolver {

    struct SimulationParameters;

    class ISensor {

    public:
        SensorDataStorage *storage = nullptr;

        SimulationParameters *simulation_parameters = nullptr;

        virtual void initialize() = 0;

        virtual void calculate_and_store(float timestep) = 0;

    };
}

#endif //FLUIDSOLVER_ISENSOR_HPP
