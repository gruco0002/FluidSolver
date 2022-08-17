#pragma once

#include "serialization/SimulationSerializer.hpp"

namespace FluidStudio {

    class ExtendedSimulationSerializer : public LibFluid::SimulationSerializer {
      public:
        using LibFluid::SimulationSerializer::SimulationSerializer;

        virtual std::shared_ptr<LibFluid::ISimulationVisualizer> deserialize_unknown_visualizer(
                const nlohmann::json&) override;

        virtual std::optional<nlohmann::json> serialize_unknown_visualizer(
                const std::shared_ptr<LibFluid::ISimulationVisualizer>&) override;
    };

} // namespace FluidStudio