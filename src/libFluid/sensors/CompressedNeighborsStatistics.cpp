#include "CompressedNeighborsStatistics.hpp"

#include "Simulator.hpp"
#include "fluidSolver/neighborhoodSearch/CompressedNeighbors.hpp"

#include <algorithm>

namespace FluidSolver::Sensors {


    void CompressedNeighborStorageSensor::calculate_and_store(const Timepoint& timepoint) {
        FLUID_ASSERT(this->simulator_data.collection != nullptr);
        FLUID_ASSERT(this->simulator_data.collection->is_type_present<CompressedNeighborhoodSearch::NeighborStorage>());

        auto& collection = this->simulator_data.collection;

        Info res;
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

        data.push_back(timepoint, res);
    }


    void CompressedNeighborStorageSensor::save_data_to_file(SensorWriter& writer) {
        if (writer.begin_header()) {
            writer.push_back_header<Timepoint>("Timepoint");
            writer.push_back_header<float>("Neighbor Count Avg");
            writer.push_back_header<float>("Neighbor Count Min");
            writer.push_back_header<float>("Neighbor Count Max");
            writer.push_back_header<float>("Delta Bytes Avg");
            writer.push_back_header<float>("Delta Bytes Min");
            writer.push_back_header<float>("Delta Bytes Max");
            writer.end_header();
        }


        // data writing
        for (size_t i = saved_data_until; i < data.size(); i++) {
            auto& tmp = data.data()[i];
            writer << data.times()[i] << tmp.neighbor_count_average << tmp.neighbor_count_minimum
                   << tmp.neighbor_count_maximum << tmp.used_delta_bytes_average << tmp.used_delta_bytes_minimum
                   << tmp.used_delta_bytes_maximum << SensorWriter::Control::Next;
        }


        if (parameters.keep_data_in_memory_after_saving) {
            saved_data_until = data.size();
        } else {
            data.clear();
            saved_data_until = 0;
        }
    }

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

} // namespace FluidSolver::Sensors