#pragma once

#include "sensors/ISensor.hpp"

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace FluidSolver
{

    class OutputManager {

      public:
        struct OutputManagerParameters
        {
            std::string output_folder = "./output";

            size_t timesteps_between_sensor_save = 10;
            std::vector<std::shared_ptr<ISensor>> sensors;
        } parameters;

        void initialize();

        void timestep_happened();

        void manual_save();

        ~OutputManager();

      private:
        size_t timesteps_since_last_sensor_save = 0;

        std::map<std::shared_ptr<ISensor>, std::shared_ptr<SensorWriter>> sensor_writers;

        void save_sensor_data();

        static std::string remove_invalid_chars_from_filename(const std::string& filename);
    };


} // namespace FluidSolver