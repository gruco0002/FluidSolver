#include "EndianSafeBinaryStream.hpp"

#include <algorithm>
#include <exception>
#include <stdexcept>

namespace FluidSolver {

    namespace EndianSwapper {
        class SwapByteBase {
          public:
            static bool is_system_little_endian() {
                static const uint16_t swap_test = 1;
                return (*((char*)&swap_test) == 1);
            }

            static bool should_swap() {
                // since most systems are little endian, we only want to swap
                // if the system is big endian. This avoids unnecessary
                // computational overhead on most common systems.
                return !is_system_little_endian();
            }

            static void swap_bytes(uint8_t& v_1, uint8_t& v_2) {
                uint8_t tmp = v_1;
                v_1 = v_2;
                v_2 = tmp;
            }
        };

        template<class T, int S>
        class SwapByte : public SwapByteBase {
          public:
            static T swap(T v) {
                return v;
            }
        };

        template<class T>
        class SwapByte<T, 1> : public SwapByteBase {
          public:
            static T swap(T v) {
                return v;
            }
        };

        template<class T>
        class SwapByte<T, 2> : public SwapByteBase {
          public:
            static T swap(T v) {
                if (should_swap())
                    return ((uint16_t)v >> 8) | ((uint16_t)v << 8);
                return v;
            }
        };

        template<class T>
        class SwapByte<T, 4> : public SwapByteBase {
          public:
            static T swap(T v) {
                if (should_swap()) {
                    return (SwapByte<uint16_t, 2>::swap((uint32_t)v & 0xffff) << 16) | (SwapByte<uint16_t, 2>::swap(((uint32_t)v & 0xffff0000) >> 16));
                }
                return v;
            }
        };

        template<class T>
        class SwapByte<T, 8> : public SwapByteBase {
          public:
            static T swap(T v) {
                if (should_swap())
                    return (((uint64_t)SwapByte<uint32_t, 4>::swap((uint32_t)(v & 0xffffffffull))) << 32) | (SwapByte<uint32_t, 4>::swap((uint32_t)(v >> 32)));
                return v;
            }
        };

    } // namespace EndianSwapper

    EndianSafeBinaryStream::EndianSafeBinaryStream(const std::filesystem::path& filepath, std::ios_base::openmode mode)
        : stream(filepath, mode | std::ios_base::binary) {
    }
    EndianSafeBinaryStream EndianSafeBinaryStream::input(const std::filesystem::path& filepath) {
        return EndianSafeBinaryStream(filepath, std::ios_base::in);
    }
    EndianSafeBinaryStream EndianSafeBinaryStream::output(const std::filesystem::path& filepath) {
        return EndianSafeBinaryStream(filepath, std::ios_base::out);
    }

    template<class T>
    T swap(const T& v) {
        return EndianSwapper::SwapByte<T, sizeof(T)>::swap(v);
    }

    template<typename type>
    void deserialize_for_integers(std::fstream& stream, type& v) {
        stream.read((char*)&v, sizeof(type));
        if (!stream) {
            throw std::runtime_error("malformed data");
        }
        v = swap(v);
    }

    template<typename type>
    void serialize_for_integers(std::fstream& stream, const type& v) {
        type swapped_v = swap(v);
        stream.write((const char*)&swapped_v, sizeof(type));
    }

    template<typename type>
    void deserialize_for_floating_point(std::fstream& stream, type& v) {
        // Custom deserializer for floating point values since converting a "swapped"
        // value back to a floating point type could cause NaN's and other problems.

        union {
            type f;
            uint8_t c[sizeof(type)] = {};
        };
        stream.read((char*)&c[0], sizeof(type));
        if (!stream) {
            throw std::runtime_error("malformed data");
        }
        if (EndianSwapper::SwapByteBase::should_swap()) {
            for (int i = 0; i < sizeof(type) / 2; ++i)
                EndianSwapper::SwapByteBase::swap_bytes(c[i], c[sizeof(type) - 1 - i]);
        }
        v = f;
    }

    template<typename type>
    void serialize_for_floating_point(std::fstream& stream, const type& v) {
        // Custom serializer for floating point values since converting a "swapped"
        // value back to a floating point type could cause NaN's and other problems.

        union {
            type f;
            uint8_t c[sizeof(type)] = {};
        };
        f = v;
        if (EndianSwapper::SwapByteBase::should_swap()) {
            for (int i = 0; i < sizeof(type) / 2; ++i)
                EndianSwapper::SwapByteBase::swap_bytes(c[i], c[sizeof(type) - 1 - i]);
        }
        stream.write((const char*)&c[0], sizeof(type));
    }

    EndianSafeBinaryStream& EndianSafeBinaryStream::operator<<(const bool& v) {
        serialize_for_integers(stream, v ? (uint8_t)1 : (uint8_t)0);
        return *this;
    }
    EndianSafeBinaryStream& EndianSafeBinaryStream::operator>>(bool& v) {
        uint8_t f;
        deserialize_for_integers(stream, f);
        if (f == 0) {
            v = false;
        } else {
            v = true;
        }
        return *this;
    }

    EndianSafeBinaryStream& EndianSafeBinaryStream::operator<<(const int8_t& v) {
        serialize_for_integers(stream, v);
        return *this;
    }
    EndianSafeBinaryStream& EndianSafeBinaryStream::operator>>(int8_t& v) {
        deserialize_for_integers(stream, v);
        return *this;
    }

    EndianSafeBinaryStream& EndianSafeBinaryStream::operator<<(const uint8_t& v) {
        serialize_for_integers(stream, v);
        return *this;
    }
    EndianSafeBinaryStream& EndianSafeBinaryStream::operator>>(uint8_t& v) {
        deserialize_for_integers(stream, v);
        return *this;
    }

    EndianSafeBinaryStream& EndianSafeBinaryStream::operator<<(const int16_t& v) {
        serialize_for_integers(stream, v);
        return *this;
    }
    EndianSafeBinaryStream& EndianSafeBinaryStream::operator>>(int16_t& v) {
        deserialize_for_integers(stream, v);
        return *this;
    }

    EndianSafeBinaryStream& EndianSafeBinaryStream::operator<<(const uint16_t& v) {
        serialize_for_integers(stream, v);
        return *this;
    }
    EndianSafeBinaryStream& EndianSafeBinaryStream::operator>>(uint16_t& v) {
        deserialize_for_integers(stream, v);
        return *this;
    }

    EndianSafeBinaryStream& EndianSafeBinaryStream::operator<<(const int32_t& v) {
        serialize_for_integers(stream, v);
        return *this;
    }
    EndianSafeBinaryStream& EndianSafeBinaryStream::operator>>(int32_t& v) {
        deserialize_for_integers(stream, v);
        return *this;
    }

    EndianSafeBinaryStream& EndianSafeBinaryStream::operator<<(const uint32_t& v) {
        serialize_for_integers(stream, v);
        return *this;
    }
    EndianSafeBinaryStream& EndianSafeBinaryStream::operator>>(uint32_t& v) {
        deserialize_for_integers(stream, v);
        return *this;
    }

    EndianSafeBinaryStream& EndianSafeBinaryStream::operator<<(const int64_t& v) {
        serialize_for_integers(stream, v);
        return *this;
    }
    EndianSafeBinaryStream& EndianSafeBinaryStream::operator>>(int64_t& v) {
        deserialize_for_integers(stream, v);
        return *this;
    }

    EndianSafeBinaryStream& EndianSafeBinaryStream::operator<<(const uint64_t& v) {
        serialize_for_integers(stream, v);
        return *this;
    }
    EndianSafeBinaryStream& EndianSafeBinaryStream::operator>>(uint64_t& v) {
        deserialize_for_integers(stream, v);
        return *this;
    }

    EndianSafeBinaryStream& EndianSafeBinaryStream::operator<<(const float& v) {
        serialize_for_floating_point(stream, v);
        return *this;
    }
    EndianSafeBinaryStream& EndianSafeBinaryStream::operator>>(float& v) {
        deserialize_for_floating_point(stream, v);
        return *this;
    }

    EndianSafeBinaryStream& EndianSafeBinaryStream::operator<<(const double& v) {
        serialize_for_floating_point(stream, v);
        return *this;
    }
    EndianSafeBinaryStream& EndianSafeBinaryStream::operator>>(double& v) {
        deserialize_for_floating_point(stream, v);
        return *this;
    }

    EndianSafeBinaryStream& EndianSafeBinaryStream::write_array(const uint8_t* v, size_t length) {
        for (size_t i = 0; i < length; i += array_buffer_size) {
            size_t length_to_write = std::min(array_buffer_size, length - i);
            stream.write(reinterpret_cast<const char*>(v + i), length_to_write);
        }
        return *this;
    }
    EndianSafeBinaryStream& EndianSafeBinaryStream::read_array(uint8_t* v, size_t length) {
        for (size_t i = 0; i < length; i += array_buffer_size) {
            size_t length_to_read = std::min(array_buffer_size, length - i);
            stream.read(reinterpret_cast<char*>(v + i), length_to_read);
        }
        return *this;
    }
} // namespace FluidSolver