#pragma once

#include "Compatibility.hpp"
#include "Forward.hpp"
#include "fluidSolver/neighborhoodSearch/NeighborhoodInterface.hpp"
#include "sensors/SensorWriter.hpp"
#include "time/ITimestepGenerator.hpp"


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
            NeighborhoodInterface* neighborhood_interface = nullptr;
            OutputManager* manager = nullptr;
        } parameters;


        virtual void initialize() = 0;

        virtual void calculate_and_store(const Timepoint& timepoint) = 0;

        virtual void save_data_to_file(SensorWriter& writer) = 0;

        // TODO: make abstract
        virtual inline Compatibility check()
        {
            return {};
        }

        virtual ~ISensor()
        {
        }

      protected:
        size_t saved_data_until = 0;
    };
} // namespace FluidSolver
