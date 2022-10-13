#include "VisualizerSerializer.hpp"
namespace LibFluid::Serialization {

    nlohmann::json VisualizerSerializer::serialize(std::shared_ptr<ISimulationVisualizer> visualizer) {
        return nlohmann::json();
    }
}