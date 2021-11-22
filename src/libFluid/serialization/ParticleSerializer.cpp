#include "ParticleSerializer.hpp"

#include <utility>


namespace esbs {
    template<>
    struct EndianSafeBinaryStreamExtension<glm::vec2> {
        template<typename Stream>
        static EndianSafeBinaryStream<Stream>& serialize(EndianSafeBinaryStream<Stream>& stream, const glm::vec2& v) {
            stream << v.x << v.y;
            return stream;
        }

        template<typename Stream>
        static EndianSafeBinaryStream<Stream>& deserialize(EndianSafeBinaryStream<Stream>& stream, glm::vec2& v) {
            stream >> v.x >> v.y;
            return stream;
        }
    };

    template<>
    struct EndianSafeBinaryStreamExtension<glm::vec3> {
        template<typename Stream>
        static EndianSafeBinaryStream<Stream>& serialize(EndianSafeBinaryStream<Stream>& stream, const glm::vec3& v) {
            stream << v.x << v.y << v.z;
            return stream;
        }

        template<typename Stream>
        static EndianSafeBinaryStream<Stream>& deserialize(EndianSafeBinaryStream<Stream>& stream, glm::vec3& v) {
            stream >> v.x >> v.y >> v.z;
            return stream;
        }
    };
} // namespace esbs


namespace FluidSolver {

    void FluidSolver::ParticleSerializer::serialize(ParticleCollection& collection) {
        esbs::EndianSafeBinaryStream<Lz4CompressedStream> stream(std::move(Lz4CompressedStream::output(filepath)));

        // version code
        stream << (uint32_t)1;

        // number of components
        stream << (uint32_t)get_amount_of_components(collection);

        // write the ids of the components
        write_component_ids(stream, collection);

        // write particle count
        stream << (uint64_t)collection.size();

        // write particle data
        for (size_t i = 0; i < collection.size(); i++) {
            // the order of writing / reading is very important!
            if (collection.is_type_present<MovementData>()) {
                const auto& v = collection.get<MovementData>(i);
                stream << v.position << v.velocity << v.acceleration;
            }
            if (collection.is_type_present<MovementData3D>()) {
                const auto& v = collection.get<MovementData3D>(i);
                stream << v.position << v.velocity << v.acceleration;
            }
            if (collection.is_type_present<ParticleInfo>()) {
                const auto& v = collection.get<ParticleInfo>(i);
                stream << v.type << v.tag;
            }
            if (collection.is_type_present<ParticleData>()) {
                const auto& v = collection.get<ParticleData>(i);
                stream << v.density << v.mass << v.pressure;
            }
            if (collection.is_type_present<ExternalForces>()) {
                const auto& v = collection.get<ExternalForces>(i);
                stream << v.non_pressure_acceleration;
            }
            if (collection.is_type_present<ExternalForces3D>()) {
                const auto& v = collection.get<ExternalForces3D>(i);
                stream << v.non_pressure_acceleration;
            }
        }
    }
    ParticleSerializer::ParticleSerializer(std::filesystem::path filepath)
        : filepath(std::move(filepath)) {
    }

    void ParticleSerializer::deserialize(ParticleCollection& collection) {
        esbs::EndianSafeBinaryStream<Lz4CompressedStream> stream(std::move(Lz4CompressedStream::input(filepath)));

        // version code
        uint32_t version;
        stream >> version;

        if (version != 1) {
            throw std::runtime_error("Unsupported particle data format version");
        }

        // read the components and add them if required
        auto available_components = read_available_components(stream);
        add_components_if_required(available_components, collection);

        // read particle count
        uint64_t particle_count;
        stream >> particle_count;

        // resize collection
        collection.resize(particle_count);

        // read particle data
        for (size_t i = 0; i < particle_count; i++) {
            FLUID_ASSERT(i < collection.size());

            // the order of writing / reading is very important!
            if (available_components.movement_data) {
                auto& v = collection.get<MovementData>(i);
                stream >> v.position >> v.velocity >> v.acceleration;
            }
            if (available_components.movement_data_3d) {
                auto& v = collection.get<MovementData3D>(i);
                stream >> v.position >> v.velocity >> v.acceleration;
            }
            if (available_components.particle_info) {
                auto& v = collection.get<ParticleInfo>(i);
                stream >> v.type >> v.tag;
            }
            if (available_components.particle_data) {
                auto& v = collection.get<ParticleData>(i);
                stream >> v.density >> v.mass >> v.pressure;
            }
            if (available_components.external_forces) {
                auto& v = collection.get<ExternalForces>(i);
                stream >> v.non_pressure_acceleration;
            }
            if (available_components.external_forces_3d) {
                auto& v = collection.get<ExternalForces3D>(i);
                stream >> v.non_pressure_acceleration;
            }
        }
    }

    size_t ParticleSerializer::get_amount_of_components(ParticleCollection& collection) {
        size_t result = 0;
        if (collection.is_type_present<MovementData>()) {
            result++;
        }
        if (collection.is_type_present<MovementData3D>()) {
            result++;
        }
        if (collection.is_type_present<ParticleInfo>()) {
            result++;
        }
        if (collection.is_type_present<ParticleData>()) {
            result++;
        }
        if (collection.is_type_present<ExternalForces>()) {
            result++;
        }
        if (collection.is_type_present<ExternalForces3D>()) {
            result++;
        }
        return result;
    }
    void ParticleSerializer::write_component_ids(esbs::EndianSafeBinaryStream<Lz4CompressedStream>& stream, ParticleCollection& collection) {
        if (collection.is_type_present<MovementData>()) {
            stream << (uint32_t)1;
        }
        if (collection.is_type_present<MovementData3D>()) {
            stream << (uint32_t)2;
        }
        if (collection.is_type_present<ParticleInfo>()) {
            stream << (uint32_t)3;
        }
        if (collection.is_type_present<ParticleData>()) {
            stream << (uint32_t)4;
        }
        if (collection.is_type_present<ExternalForces>()) {
            stream << (uint32_t)5;
        }
        if (collection.is_type_present<ExternalForces3D>()) {
            stream << (uint32_t)6;
        }
    }

    void ParticleSerializer::add_components_if_required(const ParticleSerializer::AvailableComponents& available_components, ParticleCollection& collection) {
        if (available_components.movement_data) {
            if (!collection.is_type_present<MovementData>()) {
                collection.add_type<MovementData>();
            }
        }

        if (available_components.movement_data_3d) {
            if (!collection.is_type_present<MovementData3D>()) {
                collection.add_type<MovementData3D>();
            }
        }

        if (available_components.particle_info) {
            if (!collection.is_type_present<ParticleInfo>()) {
                collection.add_type<ParticleInfo>();
            }
        }

        if (available_components.particle_data) {
            if (!collection.is_type_present<ParticleData>()) {
                collection.add_type<ParticleData>();
            }
        }

        if (available_components.external_forces) {
            if (!collection.is_type_present<ExternalForces>()) {
                collection.add_type<ExternalForces>();
            }
        }

        if (available_components.external_forces_3d) {
            if (!collection.is_type_present<ExternalForces3D>()) {
                collection.add_type<ExternalForces3D>();
            }
        }
    }
    ParticleSerializer::AvailableComponents ParticleSerializer::read_available_components(esbs::EndianSafeBinaryStream<Lz4CompressedStream>& stream) {
        AvailableComponents result;

        // read number of components
        uint32_t number_of_components;
        stream >> number_of_components;

        // check which components are active
        for (uint32_t i = 0; i < number_of_components; i++) {
            uint32_t component_id;
            stream >> component_id;

            switch (component_id) {
                case 1:
                    result.movement_data = true;
                    break;
                case 2:
                    result.movement_data_3d = true;
                    break;
                case 3:
                    result.particle_info = true;
                    break;
                case 4:
                    result.particle_data = true;
                    break;
                case 5:
                    result.external_forces = true;
                    break;
                case 6:
                    result.external_forces_3d = true;
                    break;
                default:
                    throw std::runtime_error("invalid component id");
            }
        }

        return result;
    }

} // namespace FluidSolver
