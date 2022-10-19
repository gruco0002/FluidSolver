#include "OutputManager.hpp"

#include "LibFluidAssert.hpp"

namespace LibFluid {

    OutputManager::~OutputManager() {
        sensor_streams.clear();
    }

    std::string OutputManager::remove_invalid_chars_from_filename(const std::string& filename) {
        static const std::string invalid_chars = "\\/:?\"<>|";

        std::string res;
        for (auto it = filename.begin(); it != filename.end(); ++it) {
            bool found = invalid_chars.find(*it) != std::string::npos;
            if (found) {
                res.append(" ", 1);
            } else {
                const char append = *it;
                res.append(1, append);
            }
        }
        return res;
    }

    size_t OutputManager::generate_sensor_output_identifier(const Sensor& sensor) {
        sensor_output_identifier_counter += 1;
        sensor_filenames[sensor_output_identifier_counter] = remove_invalid_chars_from_filename(sensor.parameters.filename);
        return sensor_output_identifier_counter;
    }

    std::ofstream& OutputManager::get_stream(size_t sensor_output_identifier) {
        FLUID_ASSERT(sensor_output_identifier > 0, "Invalid sensor output identifier used!");

        // check if a stream is already existing
        if (sensor_streams.find(sensor_output_identifier) == sensor_streams.end()) {
            // create the filepath for the sensor
            auto path = get_output_path();
            auto filename = sensor_filenames[sensor_output_identifier_counter];
            path = path / filename;

            // create a stream for the sensor
            sensor_streams.emplace(sensor_output_identifier, path);
        }

        // return the respective stream
        return sensor_streams[sensor_output_identifier];
    }

    std::filesystem::path OutputManager::get_output_path() const {
        auto directory = std::filesystem::path(parameters.output_folder);
        if (!std::filesystem::exists(directory)) {
            std::filesystem::create_directories(directory);
        }
        return directory;
    }


} // namespace LibFluid
