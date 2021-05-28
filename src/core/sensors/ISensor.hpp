#ifndef FLUIDSOLVER_ISENSOR_HPP
#define FLUIDSOLVER_ISENSOR_HPP

#include "core/sensors/SensorWriter.hpp"
#include "core/timestep/ITimestep.hpp"


namespace FluidSolver
{

    struct SimulationParameters;

    class ISensor {

      public:
        struct SensorParameters
        {
            std::string name = "Sensor";
            bool save_to_file = false;
            bool keep_data_in_memory_after_saving = false;
            SimulationParameters* simulation_parameters = nullptr;
        } parameters;


        virtual void initialize() = 0;

        virtual void calculate_and_store(const Timepoint& timepoint) = 0;

        virtual void save_data_to_file(SensorWriter& writer) = 0;

      protected:
        size_t saved_data_until = 0;
    };
} // namespace FluidSolver

#endif // FLUIDSOLVER_ISENSOR_HPP
