#pragma once

#include "sensors/Sensor.hpp"

#include <filesystem>
#include <fstream>
#include <map>
#include <string>

namespace LibFluid {

    class OutputManager {
      public:
        struct OutputManagerParameters {
            std::string output_folder = "./output";
        } parameters;

        ~OutputManager();

        size_t generate_sensor_output_identifier(const Sensor& sensor);

        std::ofstream& get_stream(size_t sensor_output_identifier);

      private:
        size_t sensor_output_identifier_counter = 0;

        std::map<size_t, std::ofstream> sensor_streams;
        std::map<size_t, std::string> sensor_filenames;

        static std::string remove_invalid_chars_from_filename(const std::string& filename);

        std::filesystem::path get_output_path() const;
    };


} // namespace LibFluid