#pragma once

#include "DataChangeStruct.hpp"
#include "Forward.hpp"
#include "Initializable.hpp"
#include "Reportable.hpp"
#include "time/Timepoint.hpp"


namespace LibFluid{
    class Sensor : public Initializable, public Reportable {
      public:
        struct SensorParameters {
            std::string name = "Sensor";
            bool save_to_file = false;
            bool keep_data_in_memory = false;
        } parameters;

        struct SimulatorData : public DataChangeStruct {
            std::shared_ptr<NeighborhoodInterface> neighborhood_interface = nullptr;
            std::shared_ptr<OutputManager> manager = nullptr;
            std::shared_ptr<ParticleCollection> collection = nullptr;
            std::shared_ptr<IFluidSolverBase> fluid_solver = nullptr;
        } simulator_data;


        struct SimulatorParameters : public DataChangeStruct {
            float gravity = 9.81f;
        } simulator_parameters;


        virtual void execute_timestep(const Timepoint& timepoint) = 0;

        virtual ~Sensor() = default;
    };
}