#pragma once

#include "serialization/helpers/SerializationContext.hpp"
#include "visualizer/ISimulationVisualizer.hpp"

#include <memory>
#include <nlohmann/json.hpp>

namespace LibFluid::Serialization {
    class VisualizerSerializerExtension {
      public:
        virtual bool can_serialize(const std::shared_ptr<ISimulationVisualizer>& visualizer) const = 0;
        virtual nlohmann::json serialize_visualizer(const std::shared_ptr<ISimulationVisualizer>& visualizer, SerializationContext& context) const = 0;
    };
} // namespace LibFluid::Serialization