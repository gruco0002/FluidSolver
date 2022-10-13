#pragma once

#include "SimulatorVisualizerBundle.hpp"
#include "serialization/extensions/SerializerExtensions.hpp"
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
            SerializeSettings(bool save_particle_data, const std::string& particle_data_relative_filepath)
                : save_particle_data(save_particle_data), particle_data_relative_filepath(particle_data_relative_filepath) {}
        };

      public:
        explicit MainSerializer(const std::filesystem::path& filepath);
        explicit MainSerializer(const std::filesystem::path& filepath, SerializerExtensions serializer_extensions);

        void serialize_bundle(SimulatorVisualizerBundle& bundle, const SerializeSettings& settings = SerializeSettings(), SerializationContext* = nullptr);
        void serialize_simulator_only(std::shared_ptr<Simulator> simulator, const SerializeSettings& settings = SerializeSettings(), SerializationContext* = nullptr);

        SimulatorVisualizerBundle deserialize(SerializationContext* = nullptr);


      private:
        std::filesystem::path filepath;
        SerializerExtensions serializer_extensions;

        std::filesystem::path get_full_particle_data_path(const SerializationContext& context) const;
    };
} // namespace LibFluid::Serialization
