#pragma once

#include "LibFluidForward.hpp"
#include "helpers/CompatibilityReport.hpp"
#include "sensors/OutputManager.hpp"
#include "sensors/Sensor.hpp"
#include "sensors/SensorDataStore.hpp"

#include <fmt/chrono.h>
#include <fstream>
#include <nlohmann/json.hpp>

namespace LibFluid
{

    struct SensorDataFieldDefinition
    {
        std::string field_name;
        enum class FieldType
        {
            Int,
            Float,
            String,
            Bool,
            Custom
        } type;
        std::string description;
        std::string unit;
    };

    template <typename T> class SensorBase : public Sensor
    {
      public:
      private:
        SensorDataStore<T> sensor_data_store;
        T last_data;
        size_t sensor_output_identifier = 0;

        nlohmann::json generate_header()
        {
            nlohmann::json header = nlohmann::json::object();

            header["name"] = parameters.name;
            header["definitions"] = nlohmann::json::array();

            // add definitions
            auto definitions = get_definitions();
            for (auto &def : definitions)
            {
                nlohmann::json json_def = nlohmann::json::object();
                json_def["fieldName"] = def.field_name;
                json_def["description"] = def.description;
                json_def["unit"] = def.unit;

                switch (def.type)
                {
                case SensorDataFieldDefinition::FieldType::Int:
                    json_def["type"] = "int";
                    break;
                case SensorDataFieldDefinition::FieldType::Float:
                    json_def["type"] = "float";
                    break;
                case SensorDataFieldDefinition::FieldType::String:
                    json_def["type"] = "string";
                    break;
                case SensorDataFieldDefinition::FieldType::Bool:
                    json_def["type"] = "boolean";
                    break;
                case SensorDataFieldDefinition::FieldType::Custom:
                    json_def["type"] = "custom";
                    break;
                }

                header["definitions"].push_back(json_def);
            }

            return header;
        }

        static nlohmann::json serialize_timepoint(const Timepoint &timepoint)
        {
            nlohmann::json j = nlohmann::json::object();
            j["actualTimestep"] = timepoint.actual_time_step;
            j["desiredTimestep"] = timepoint.desired_time_step;
            j["simulationTime"] = timepoint.simulation_time;
            j["timestepNumber"] = timepoint.timestep_number;
            j["systemTime"] = fmt::format("{:%Y-%m-%dT%H:%M:%SZ}", fmt::gmtime(timepoint.system_time));
            return j;
        }

      public:
        virtual std::vector<SensorDataFieldDefinition> get_definitions() = 0;

        virtual T calculate_for_timepoint(const Timepoint &timepoint) = 0;

        virtual void add_data_fields_to_json_array(nlohmann::json &array, const T &data) = 0;

        const SensorDataStore<T> &get_sensor_data_store() const
        {
            return sensor_data_store;
        }

        const T &get_last_data() const
        {
            return last_data;
        }

        void execute_timestep(const Timepoint &timepoint) override
        {
            if (!parameters.keep_data_in_memory)
            {
                // clear the sensor data store
                sensor_data_store.clear();
            }

            // calculate the new sensor data for the current timepoint
            auto data = calculate_for_timepoint(timepoint);

            // update the last data
            last_data = data;

            // store the sensor data if required
            if (parameters.keep_data_in_memory)
            {
                sensor_data_store.push_back(timepoint, data);
            }

            // save the sensor data to file if required
            if (parameters.save_to_file)
            {
                if (sensor_output_identifier == 0)
                {
                    // the sensor never wrote something to a file

                    // retrieve a unique id
                    sensor_output_identifier = simulator_data.manager->generate_sensor_output_identifier(*this);

                    // generate the header
                    auto header = generate_header();

                    // get an ostream from the output manager and write the header to it
                    auto &stream = simulator_data.manager->get_stream(sensor_output_identifier);
                    stream << std::setw(0) << header << std::endl;
                }

                // create the output array
                nlohmann::json data_output = nlohmann::json::array();

                // add the timepoint to it
                auto serialized_timepoint = serialize_timepoint(timepoint);
                data_output.push_back(serialized_timepoint);

                // serialize the sensor data itself into the json array
                add_data_fields_to_json_array(data_output, data);

                // get the ostream for the output manager and write the current sensor data to it
                auto &stream = simulator_data.manager->get_stream(sensor_output_identifier);
                stream << std::setw(0) << data_output << std::endl;
            }
        }

      public:
        virtual ~SensorBase() = default;

        // TODO: make abstract
        void create_compatibility_report(CompatibilityReport &report) override
        {
        }

        void initialize() override
        {
        }
    };
} // namespace LibFluid
