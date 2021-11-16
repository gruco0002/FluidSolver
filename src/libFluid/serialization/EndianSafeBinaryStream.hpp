#pragma once

#include <filesystem>
#include <fstream>

#define ENDIAN_SAFE_BINARY_STREAM_DEFAULT_EXTENSIONS
#ifdef ENDIAN_SAFE_BINARY_STREAM_DEFAULT_EXTENSIONS
    #include <string>
    #include <vector>
#endif


namespace FluidSolver {

    template<typename T>
    struct EndianSafeBinaryStreamExtension;

    /**
     * An endian safe binary stream that reads/writes primitive types always
     * as little-endian encoded to a file independent of the current system / compiler.
     * It can be expanded via explicit template instantiation of the
     * EndianSafeBinaryStreamExtension struct.
     *
     * This class is inspired by https://github.com/voidah/archive
     */
    class EndianSafeBinaryStream {


      public:
        explicit EndianSafeBinaryStream(const std::filesystem::path& filepath, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out);

        static EndianSafeBinaryStream input(const std::filesystem::path& filepath);
        static EndianSafeBinaryStream output(const std::filesystem::path& filepath);


        EndianSafeBinaryStream& operator<<(const bool& v);
        EndianSafeBinaryStream& operator>>(bool& v);

        EndianSafeBinaryStream& operator<<(const int8_t& v);
        EndianSafeBinaryStream& operator>>(int8_t& v);

        EndianSafeBinaryStream& operator<<(const uint8_t& v);
        EndianSafeBinaryStream& operator>>(uint8_t& v);

        EndianSafeBinaryStream& operator<<(const int16_t& v);
        EndianSafeBinaryStream& operator>>(int16_t& v);

        EndianSafeBinaryStream& operator<<(const uint16_t& v);
        EndianSafeBinaryStream& operator>>(uint16_t& v);

        EndianSafeBinaryStream& operator<<(const int32_t& v);
        EndianSafeBinaryStream& operator>>(int32_t& v);

        EndianSafeBinaryStream& operator<<(const uint32_t& v);
        EndianSafeBinaryStream& operator>>(uint32_t& v);

        EndianSafeBinaryStream& operator<<(const int64_t& v);
        EndianSafeBinaryStream& operator>>(int64_t& v);

        EndianSafeBinaryStream& operator<<(const uint64_t& v);
        EndianSafeBinaryStream& operator>>(uint64_t& v);

        EndianSafeBinaryStream& operator<<(const float& v);
        EndianSafeBinaryStream& operator>>(float& v);

        EndianSafeBinaryStream& operator<<(const double& v);
        EndianSafeBinaryStream& operator>>(double& v);

        EndianSafeBinaryStream& write_array(const uint8_t* v, size_t length);
        EndianSafeBinaryStream& read_array(uint8_t* v, size_t length);

      public:
        // this is for custom extensions
        template<typename T>
        EndianSafeBinaryStream& operator<<(const T& v) {
            return EndianSafeBinaryStreamExtension<T>::serialize(*this, v);
        }

        template<typename T>
        EndianSafeBinaryStream& operator>>(T& v) {
            return EndianSafeBinaryStreamExtension<T>::deserialize(*this, v);
        }

      private:
        std::fstream stream;

        static constexpr size_t array_buffer_size = 4096;
    };


// Default extensions following, you do not need to use them,
// just remove the defined macro from the top of this file
#ifdef ENDIAN_SAFE_BINARY_STREAM_DEFAULT_EXTENSIONS

    template<>
    struct EndianSafeBinaryStreamExtension<std::string> {
        static EndianSafeBinaryStream& serialize(EndianSafeBinaryStream& stream, const std::string& v) {
            uint64_t string_length = v.size();
            stream << string_length;
            stream.write_array(reinterpret_cast<const uint8_t*>(v.data()), v.size());
            return stream;
        }
        static EndianSafeBinaryStream& deserialize(EndianSafeBinaryStream& stream, std::string& v) {
            uint64_t string_length;
            stream >> string_length;
            v.resize(string_length);
            stream.read_array(reinterpret_cast<uint8_t*>(v.data()), v.size());
            return stream;
        }
    };

    template<typename S>
    struct EndianSafeBinaryStreamExtension<std::vector<S>> {
        static EndianSafeBinaryStream& serialize(EndianSafeBinaryStream& stream, const std::vector<S>& v) {
            uint64_t vector_length = v.size();
            stream << vector_length;
            for (size_t i = 0; i < v.size(); i++) {
                stream << v[i];
            }
            return stream;
        }
        static EndianSafeBinaryStream& deserialize(EndianSafeBinaryStream& stream, std::vector<S>& v) {
            uint64_t vector_length;
            stream >> vector_length;
            v.resize(vector_length);
            for (size_t i = 0; i < v.size(); i++) {
                stream >> v[i];
            }
            return stream;
        }
    };

#endif

} // namespace FluidSolver
