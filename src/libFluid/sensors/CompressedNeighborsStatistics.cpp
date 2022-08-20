#include "CompressedNeighborsStatistics.hpp"

#include "Simulator.hpp"
#include "fluidSolver/neighborhoodSearch/CompressedNeighbors.hpp"

#include <algorithm>

namespace LibFluid::Sensors {


    void CompressedNeighborStorageSensor::create_compatibility_report(CompatibilityReport& report) {
        report.begin_scope(FLUID_NAMEOF(CompressedNeighborStorageSensor));
        if (this->simulator_data.collection == nullptr) {
            report.add_issue("Particle collection is nullptr.");
        } else {
            if (!this->simulator_data.collection->is_type_present<CompressedNeighborhoodSearch::NeighborStorage>()) {
                report.add_issue(
                        "NeighborStorage attribute is missing in particle collection.");
            }
        }
        report.end_scope();
    }
    std::vector<SensorDataFieldDefinition> CompressedNeighborStorageSensor::get_definitions() {
        return {
                {"Neighbor Count Average",
                        SensorDataFieldDefinition::FieldType::Float,
                        "",
                        ""},
                {"Neighbor Count Minimum",
                        SensorDataFieldDefinition::FieldType::Float,
                        "",
                        ""},
                {"Neighbor Count Maximum",
                        SensorDataFieldDefinition::FieldType::Float,
                        "",
                        ""},
                {"Used Delta Bytes Average",
                        SensorDataFieldDefinition::FieldType::Float,
                        "",
                        ""},
                {"Used Delta Bytes Minimum",
                        SensorDataFieldDefinition::FieldType::Float,
                        "",
                        ""},
                {"Used Delta Bytes Maximum",
                        SensorDataFieldDefinition::FieldType::Float,
                        "",
                        ""},

        };
    }

    CompressedNeighborStorageSensorInfo CompressedNeighborStorageSensor::calculate_for_timepoint(const Timepoint& timepoint) {
        FLUID_ASSERT(this->simulator_data.collection != nullptr);
        FLUID_ASSERT(this->simulator_data.collection->is_type_present<CompressedNeighborhoodSearch::NeighborStorage>());

        auto& collection = this->simulator_data.collection;

        CompressedNeighborStorageSensorInfo res;
        size_t counter = 0;
        for (size_t i = 0; i < collection->size(); i++) {
            const auto& storage = collection->get<CompressedNeighborhoodSearch::NeighborStorage>(i);

            float neighbors = storage.size();
            float bytes = storage.get_used_delta_bytes();

            res.neighbor_count_average += neighbors;
            res.neighbor_count_maximum = std::max(res.neighbor_count_maximum, neighbors);
            res.neighbor_count_minimum = std::min(res.neighbor_count_minimum, neighbors);

            res.used_delta_bytes_average += bytes;
            res.used_delta_bytes_maximum = std::max(res.used_delta_bytes_maximum, bytes);
            res.used_delta_bytes_minimum = std::min(res.used_delta_bytes_minimum, bytes);

            counter++;
        }
        if (counter != 0) {
            res.neighbor_count_average /= counter;
            res.used_delta_bytes_average /= counter;
        }

        return res;
    }

    void CompressedNeighborStorageSensor::add_data_fields_to_json_array(nlohmann::json& array, const CompressedNeighborStorageSensorInfo& data) {
        array.push_back(data.neighbor_count_average);
        array.push_back(data.neighbor_count_minimum);
        array.push_back(data.neighbor_count_maximum);
        array.push_back(data.used_delta_bytes_average);
        array.push_back(data.used_delta_bytes_minimum);
        array.push_back(data.used_delta_bytes_maximum);
    }

} // namespace LibFluid::Sensors