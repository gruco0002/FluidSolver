#pragma once

#include "serialization/extensions/SerializerExtensions.hpp"
#include "serialization/extensions/VisualizerSerializerExtension.hpp"

namespace FluidConsole {
    namespace FluidConsoleSerializerExtensions {
        LibFluid::Serialization::SerializerExtensions create_extensions();
    }

    class IgnoreUnkownVisualizerExtension final : public LibFluid::Serialization::VisualizerSerializerExtension {
      public:
        bool can_serialize(const std::shared_ptr<LibFluid::ISimulationVisualizer>& visualizer) const override;
        nlohmann::json serialize_visualizer(const std::shared_ptr<LibFluid::ISimulationVisualizer>& visualizer, LibFluid::Serialization::SerializationContext& context) const override;
        bool can_deserialize(const nlohmann::json& node) const override;
        std::shared_ptr<LibFluid::ISimulationVisualizer> deserialize_visualizer(const nlohmann::json& node, LibFluid::Serialization::SerializationContext& context) const override;
    };
} // namespace FluidConsole
