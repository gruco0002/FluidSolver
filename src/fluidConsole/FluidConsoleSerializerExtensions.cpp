#include "FluidConsoleSerializerExtensions.hpp"
namespace FluidConsole {

    bool IgnoreUnkownVisualizerExtension::can_serialize(const std::shared_ptr<LibFluid::ISimulationVisualizer>& visualizer) const {
        return false;
    }
    nlohmann::json IgnoreUnkownVisualizerExtension::serialize_visualizer(const std::shared_ptr<LibFluid::ISimulationVisualizer>& visualizer, LibFluid::Serialization::SerializationContext& context) const {
        return nullptr;
    }
    bool IgnoreUnkownVisualizerExtension::can_deserialize(const nlohmann::json& node) const {
        return true;
    }
    std::shared_ptr<LibFluid::ISimulationVisualizer> IgnoreUnkownVisualizerExtension::deserialize_visualizer(const nlohmann::json& node, LibFluid::Serialization::SerializationContext& context) const {
        return nullptr;
    }
    LibFluid::Serialization::SerializerExtensions FluidConsoleSerializerExtensions::create_extensions() {
        LibFluid::Serialization::SerializerExtensions extensions;

        extensions.visualizer_serializer_extensions.push_back(std::make_shared<IgnoreUnkownVisualizerExtension>());

        return extensions;
    }
} // namespace FluidConsole