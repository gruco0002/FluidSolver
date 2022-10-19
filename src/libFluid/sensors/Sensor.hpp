#pragma once

#include "LibFluidForward.hpp"
#include "helpers/DataChangeStruct.hpp"
#include "helpers/Initializable.hpp"
#include "helpers/Reportable.hpp"
#include "time/Timepoint.hpp"

#include <memory>


namespace LibFluid {
    class Sensor : public Initializable, public Reportable {
      public:
        struct SensorParameters {
            std::string name = "Sensor";
            bool keep_data_in_memory = false;

            bool save_to_file = false;
            std::string filename = "xyz.sensor";
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
} // namespace LibFluid