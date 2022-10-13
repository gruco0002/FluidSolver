#pragma once

#include "SimulatorVisualizerBundle.hpp"
#include "serialization/helpers/SerializationContext.hpp"

#include <filesystem>
#include <string>

namespace LibFluid::Serialization {
    class MainSerializer {
      public:
        struct SerializeSettings {
            bool save_particle_data;
            std::string particle_data_relative_filepath;

            SerializeSettings()
                : save_particle_data(true), particle_data_relative_filepath("particles.data") {}
        };

        explicit MainSerializer(const std::filesystem::path& filepath);

        void serialize_bundle(SimulatorVisualizerBundle& bundle, const SerializeSettings& settings = SerializeSettings(), SerializationContext* = nullptr);
        void serialize_simulator_only(std::shared_ptr<Simulator> simulator, const SerializeSettings& settings = SerializeSettings(), SerializationContext* = nullptr);

        SimulatorVisualizerBundle deserialize(SerializationContext* = nullptr);

      private:
        std::filesystem::path filepath;

        std::filesystem::path get_full_particle_data_path(const SerializationContext& context) const;
    };
} // namespace LibFluid::Serialization
