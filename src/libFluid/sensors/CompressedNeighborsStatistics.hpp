#pragma once

#include "FluidInclude.hpp"
#include "sensors/ISensor.hpp"
#include "sensors/SensorData.hpp"

#include <limits>


namespace FluidSolver::Sensors
{

    class CompressedNeighborStorageSensor : public ISensor {
      public:
        struct Info
        {
            float used_delta_bytes_average = 0.0f;
            float used_delta_bytes_minimum = std::numeric_limits<float>::max();
            float used_delta_bytes_maximum = std::numeric_limits<float>::lowest();

            float neighbor_count_average = 0.0f;
            float neighbor_count_minimum = std::numeric_limits<float>::max();
            float neighbor_count_maximum = std::numeric_limits<float>::lowest();
        };

        SensorData<Info> data;

        virtual void initialize() override;
        virtual void calculate_and_store(const Timepoint& timepoint) override;
        virtual void save_data_to_file(SensorWriter& writer) override;

        virtual Compatibility check() override;
    };

} // namespace FluidSolver::Sensors