#pragma once

#include "fluidSolver/ParticleCollection.hpp"
#include "serialization/helpers/EndianSafeBinaryStream.hpp"
#include "serialization/helpers/Lz4CompressedStream.hpp"

#include <filesystem>

namespace LibFluid::Serialization {

    class ParticleSerializer {
      public:
        explicit ParticleSerializer(std::filesystem::path filepath);

        void serialize(ParticleCollection& collection);
        void deserialize(ParticleCollection& collection);

      private:
        struct AvailableComponents {
            bool movement_data = false;
            bool movement_data_3d = false;
            bool particle_info = false;
            bool particle_data = false;
            bool external_forces = false;
            bool external_forces_3d = false;
        };

        std::filesystem::path filepath;

        size_t get_amount_of_components(ParticleCollection& collection);
        void write_component_ids(esbs::EndianSafeBinaryStream<Lz4CompressedStream>& stream, ParticleCollection& collection);

        AvailableComponents read_available_components(esbs::EndianSafeBinaryStream<Lz4CompressedStream>& stream);
        void add_components_if_required(const AvailableComponents& available_components, ParticleCollection& collection);
    };

} // namespace FluidSolver
