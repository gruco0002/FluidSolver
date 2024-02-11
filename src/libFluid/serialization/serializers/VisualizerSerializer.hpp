#pragma once

#include "serialization/serializers/SerializerBase.hpp"
#include "visualizer/ISimulationVisualizer.hpp"

#include <memory>
#include <nlohmann/json.hpp>

namespace LibFluid::Serialization
{
    class VisualizerSerializer final : public SerializerBase
    {
      public:
        using SerializerBase::SerializerBase;

        nlohmann::json serialize(std::shared_ptr<ISimulationVisualizer> visualizer);

        std::shared_ptr<ISimulationVisualizer> deserialize(const nlohmann::json &node);

      private:
        nlohmann::json serialize_continuous_visualizer(std::shared_ptr<ISimulationVisualizer> visualizer);
        std::shared_ptr<ISimulationVisualizer> deserialize_continuous_visualizer(const nlohmann::json &node);
        nlohmann::json serialize_raytracer(std::shared_ptr<ISimulationVisualizer> visualizer);
        std::shared_ptr<ISimulationVisualizer> deserialize_raytracer(const nlohmann::json &node);
        nlohmann::json serialize_simple_raytracer(std::shared_ptr<ISimulationVisualizer> visualizer);
        std::shared_ptr<ISimulationVisualizer> deserialize_simple_raytracer(const nlohmann::json &node);
    };
} // namespace LibFluid::Serialization
