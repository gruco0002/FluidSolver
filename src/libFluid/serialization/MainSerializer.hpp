#pragma once

#include "SimulatorVisualizerBundle.hpp"
#include "serialization/helpers/SerializationContext.hpp"

#include <filesystem>

namespace LibFluid::Serialization {
    class MainSerializer {
      public:
        explicit MainSerializer(const std::filesystem::path& filepath);

        void serialize_bundle(SimulatorVisualizerBundle& bundle, SerializationContext* = nullptr);
        void serialize_simulator_only(std::shared_ptr<Simulator> simulator, SerializationContext* = nullptr);

        SimulatorVisualizerBundle deserialize(SerializationContext* = nullptr);

      private:

        std::filesystem::path filepath;

    };
}
