#pragma once

#include "serialization/extensions/VisualizerSerializerExtension.hpp"

namespace FluidStudio {
    class VseGlParticleRenderer final : public LibFluid::Serialization::VisualizerSerializerExtension {
      public:
        bool can_serialize(const std::shared_ptr<LibFluid::ISimulationVisualizer>& visualizer) const override;
        nlohmann::json serialize_visualizer(const std::shared_ptr<LibFluid::ISimulationVisualizer>& visualizer, LibFluid::Serialization::SerializationContext& context) const override;
    };
} // namespace FluidStudio
