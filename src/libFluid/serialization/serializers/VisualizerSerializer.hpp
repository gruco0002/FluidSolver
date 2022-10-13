#pragma once

#include "serialization/serializers/SerializerBase.hpp"
#include "visualizer/ISimulationVisualizer.hpp"

#include <memory>
#include <nlohmann/json.hpp>

namespace LibFluid::Serialization {
    class VisualizerSerializer final : public SerializerBase {
      public:
        using SerializerBase::SerializerBase;

        nlohmann::json serialize(std::shared_ptr<ISimulationVisualizer> visualizer);
    };
} // namespace LibFluid::Serialization
