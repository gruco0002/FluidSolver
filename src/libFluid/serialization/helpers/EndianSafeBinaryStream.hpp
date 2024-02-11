#pragma once

#include <algorithm>
#include <exception>
#include <stdexcept>

// Undefine this if you do not want to use the default extensions written for
// std::string and std::vector
#define ENDIAN_SAFE_BINARY_STREAM_DEFAULT_EXTENSIONS
#ifdef ENDIAN_SAFE_BINARY_STREAM_DEFAULT_EXTENSIONS
#include <string>
#include <vector>
#endif

// Undefine this if you do not want to use the helper functions for std::fstream
#define ENDIAN_SAFE_BINARY_STREAM_FSTREAM_HELPER
#ifdef ENDIAN_SAFE_BINARY_STREAM_FSTREAM_HELPER
#include <filesystem>
#include <fstream>
#endif

namespace esbs
{

    template <typename T> struct EndianSafeBinaryStreamExtension;

    /**
     * An endian safe binary stream that reads/writes primitive types always
     * as little-endian encoded to a stream independent of the current system / compiler.
     * It can be expanded via explicit template instantiation of the
     * EndianSafeBinaryStreamExtension struct.
     *
     * The Stream template parameter should be instantiated to a type that supports the
     * read, write and !operator, like for example std::fstream.
     * If EndianSafeBinaryStream is only used for writing, it is sufficient that the Stream
     * supports the write function.
     * If EndianSafeBinaryStream is only used for reading, the Stream template parameter
     * should have an implementation of the read function and the !operator.
     */
    template <typename Stream> class EndianSafeBinaryStream
    {

      public:
        explicit EndianSafeBinaryStream(const Stream &stream);
        explicit EndianSafeBinaryStream(Stream &&stream);

        EndianSafeBinaryStream &operator<<(const bool &v);
        EndianSafeBinaryStream &operator>>(bool &v);

        EndianSafeBinaryStream &operator<<(const int8_t &v);
        EndianSafeBinaryStream &operator>>(int8_t &v);

        EndianSafeBinaryStream &operator<<(const uint8_t &v);
        EndianSafeBinaryStream &operator>>(uint8_t &v);

        EndianSafeBinaryStream &operator<<(const int16_t &v);
        EndianSafeBinaryStream &operator>>(int16_t &v);

        EndianSafeBinaryStream &operator<<(const uint16_t &v);
        EndianSafeBinaryStream &operator>>(uint16_t &v);

        EndianSafeBinaryStream &operator<<(const int32_t &v);
        EndianSafeBinaryStream &operator>>(int32_t &v);

        EndianSafeBinaryStream &operator<<(const uint32_t &v);
        EndianSafeBinaryStream &operator>>(uint32_t &v);

        EndianSafeBinaryStream &operator<<(const int64_t &v);
        EndianSafeBinaryStream &operator>>(int64_t &v);

        EndianSafeBinaryStream &operator<<(const uint64_t &v);
        EndianSafeBinaryStream &operator>>(uint64_t &v);

        EndianSafeBinaryStream &operator<<(const float &v);
        EndianSafeBinaryStream &operator>>(float &v);

        EndianSafeBinaryStream &operator<<(const double &v);
        EndianSafeBinaryStream &operator>>(double &v);

        EndianSafeBinaryStream &write_array(const uint8_t *v, size_t length);
        EndianSafeBinaryStream &read_array(uint8_t *v, size_t length);

      public:
        // this is for custom extensions
        template <typename T> EndianSafeBinaryStream &operator<<(const T &v)
        {
            return EndianSafeBinaryStreamExtension<T>::template serialize<Stream>(*this, v);
        }

        template <typename T> EndianSafeBinaryStream &operator>>(T &v)
        {
            return EndianSafeBinaryStreamExtension<T>::template deserialize<Stream>(*this, v);
        }

      private:
        Stream stream;

        static constexpr size_t array_buffer_size = 4096;
    };

// Default helper functions for file streams
#ifdef ENDIAN_SAFE_BINARY_STREAM_FSTREAM_HELPER
    inline EndianSafeBinaryStream<std::fstream> input_from_file(const std::filesystem::path &filepath)
    {
        return EndianSafeBinaryStream<std::fstream>(std::fstream(filepath, std::ios_base::binary | std::ios_base::in));
    }
    inline EndianSafeBinaryStream<std::fstream> output_to_file(const std::filesystem::path &filepath)
    {
        return EndianSafeBinaryStream<std::fstream>(std::fstream(filepath, std::ios_base::binary | std::ios_base::out));
    }
#endif

// Default extensions following, you do not need to use them,
// just remove the define macro from the top of this file
#ifdef ENDIAN_SAFE_BINARY_STREAM_DEFAULT_EXTENSIONS

    template <> struct EndianSafeBinaryStreamExtension<std::string>
    {
        template <typename Stream>
        static EndianSafeBinaryStream<Stream> &serialize(EndianSafeBinaryStream<Stream> &stream, const std::string &v)
        {
            uint64_t string_length = v.size();
            stream << string_length;
            stream.write_array(reinterpret_cast<const uint8_t *>(v.data()), v.size());
            return stream;
        }

        template <typename Stream>
        static EndianSafeBinaryStream<Stream> &deserialize(EndianSafeBinaryStream<Stream> &stream, std::string &v)
        {
            uint64_t string_length;
            stream >> string_length;
            v.resize(string_length);
            stream.read_array(reinterpret_cast<uint8_t *>(v.data()), v.size());
            return stream;
        }
    };

    template <typename S> struct EndianSafeBinaryStreamExtension<std::vector<S>>
    {
        template <typename Stream>
        static EndianSafeBinaryStream<Stream> &serialize(EndianSafeBinaryStream<Stream> &stream,
                                                         const std::vector<S> &v)
        {
            uint64_t vector_length = v.size();
            stream << vector_length;
            for (size_t i = 0; i < v.size(); i++)
            {
                stream << v[i];
            }
            return stream;
        }

        template <typename Stream>
        static EndianSafeBinaryStream<Stream> &deserialize(EndianSafeBinaryStream<Stream> &stream, std::vector<S> &v)
        {
            uint64_t vector_length;
            stream >> vector_length;
            v.resize(vector_length);
            for (size_t i = 0; i < v.size(); i++)
            {
                stream >> v[i];
            }
            return stream;
        }
    };

#endif

    // Implementation of other functions and helpers below

    namespace EndianSwapper
    {
        class SwapByteBase
        {
          public:
            static bool is_system_little_endian()
            {
                static const uint16_t swap_test = 1;
                return (*((char *)&swap_test) == 1);
            }

            static bool should_swap()
            {
                // since most systems are little endian, we only want to swap
                // if the system is big endian. This avoids unnecessary
                // computational overhead on most common systems.
                return !is_system_little_endian();
            }

            static void swap_bytes(uint8_t &v_1, uint8_t &v_2)
            {
                uint8_t tmp = v_1;
                v_1 = v_2;
                v_2 = tmp;
            }
        };

        template <class T, int S> class SwapByte : public SwapByteBase
        {
          public:
            static T swap(T v)
            {
                return v;
            }
        };

        template <class T> class SwapByte<T, 1> : public SwapByteBase
        {
          public:
            static T swap(T v)
            {
                return v;
            }
        };

        template <class T> class SwapByte<T, 2> : public SwapByteBase
        {
          public:
            static T swap(T v)
            {
                if (should_swap())
                    return ((uint16_t)v >> 8) | ((uint16_t)v << 8);
                return v;
            }
        };

        template <class T> class SwapByte<T, 4> : public SwapByteBase
        {
          public:
            static T swap(T v)
            {
                if (should_swap())
                {
                    return (SwapByte<uint16_t, 2>::swap((uint32_t)v & 0xffff) << 16) |
                           (SwapByte<uint16_t, 2>::swap(((uint32_t)v & 0xffff0000) >> 16));
                }
                return v;
            }
        };

        template <class T> class SwapByte<T, 8> : public SwapByteBase
        {
          public:
            static T swap(T v)
            {
                if (should_swap())
                    return (((uint64_t)SwapByte<uint32_t, 4>::swap((uint32_t)(v & 0xffffffffull))) << 32) |
                           (SwapByte<uint32_t, 4>::swap((uint32_t)(v >> 32)));
                return v;
            }
        };

    } // namespace EndianSwapper

    namespace Helper
    {
        template <class T> T swap(const T &v)
        {
            return EndianSwapper::SwapByte<T, sizeof(T)>::swap(v);
        }

        template <typename type, typename Stream> void deserialize_for_integers(Stream &stream, type &v)
        {
            stream.read((char *)&v, sizeof(type));
            if (!stream)
            {
                throw std::runtime_error("malformed data");
            }
            v = swap(v);
        }

        template <typename type, typename Stream> void serialize_for_integers(Stream &stream, const type &v)
        {
            type swapped_v = swap(v);
            stream.write((const char *)&swapped_v, sizeof(type));
        }

        template <typename type, typename Stream> void deserialize_for_floating_point(Stream &stream, type &v)
        {
            // Custom deserializer for floating point values since converting a "swapped"
            // value back to a floating point type could cause NaN's and other problems.

            union {
                type f;
                uint8_t c[sizeof(type)] = {};
            };
            stream.read((char *)&c[0], sizeof(type));
            if (!stream)
            {
                throw std::runtime_error("malformed data");
            }
            if (EndianSwapper::SwapByteBase::should_swap())
            {
                for (int i = 0; i < sizeof(type) / 2; ++i)
                    EndianSwapper::SwapByteBase::swap_bytes(c[i], c[sizeof(type) - 1 - i]);
            }
            v = f;
        }

        template <typename type, typename Stream> void serialize_for_floating_point(Stream &stream, const type &v)
        {
            // Custom serializer for floating point values since converting a "swapped"
            // value back to a floating point type could cause NaN's and other problems.

            union {
                type f;
                uint8_t c[sizeof(type)] = {};
            };
            f = v;
            if (EndianSwapper::SwapByteBase::should_swap())
            {
                for (int i = 0; i < sizeof(type) / 2; ++i)
                    EndianSwapper::SwapByteBase::swap_bytes(c[i], c[sizeof(type) - 1 - i]);
            }
            stream.write((const char *)&c[0], sizeof(type));
        }
    } // namespace Helper

    template <typename Stream>
    EndianSafeBinaryStream<Stream>::EndianSafeBinaryStream(const Stream &stream) : stream(stream)
    {
    }

    template <typename Stream>
    EndianSafeBinaryStream<Stream>::EndianSafeBinaryStream(Stream &&stream) : stream(std::move(stream))
    {
    }

    template <typename Stream> EndianSafeBinaryStream<Stream> &EndianSafeBinaryStream<Stream>::operator<<(const bool &v)
    {
        Helper::serialize_for_integers(stream, v ? (uint8_t)1 : (uint8_t)0);
        return *this;
    }

    template <typename Stream> EndianSafeBinaryStream<Stream> &EndianSafeBinaryStream<Stream>::operator>>(bool &v)
    {
        uint8_t f;
        Helper::deserialize_for_integers(stream, f);
        if (f == 0)
        {
            v = false;
        }
        else
        {
            v = true;
        }
        return *this;
    }

    template <typename Stream>
    EndianSafeBinaryStream<Stream> &EndianSafeBinaryStream<Stream>::operator<<(const int8_t &v)
    {
        Helper::serialize_for_integers(stream, v);
        return *this;
    }

    template <typename Stream> EndianSafeBinaryStream<Stream> &EndianSafeBinaryStream<Stream>::operator>>(int8_t &v)
    {
        Helper::deserialize_for_integers(stream, v);
        return *this;
    }

    template <typename Stream>
    EndianSafeBinaryStream<Stream> &EndianSafeBinaryStream<Stream>::operator<<(const uint8_t &v)
    {
        Helper::serialize_for_integers(stream, v);
        return *this;
    }

    template <typename Stream> EndianSafeBinaryStream<Stream> &EndianSafeBinaryStream<Stream>::operator>>(uint8_t &v)
    {
        Helper::deserialize_for_integers(stream, v);
        return *this;
    }

    template <typename Stream>
    EndianSafeBinaryStream<Stream> &EndianSafeBinaryStream<Stream>::operator<<(const int16_t &v)
    {
        Helper::serialize_for_integers(stream, v);
        return *this;
    }

    template <typename Stream> EndianSafeBinaryStream<Stream> &EndianSafeBinaryStream<Stream>::operator>>(int16_t &v)
    {
        Helper::deserialize_for_integers(stream, v);
        return *this;
    }

    template <typename Stream>
    EndianSafeBinaryStream<Stream> &EndianSafeBinaryStream<Stream>::operator<<(const uint16_t &v)
    {
        Helper::serialize_for_integers(stream, v);
        return *this;
    }

    template <typename Stream> EndianSafeBinaryStream<Stream> &EndianSafeBinaryStream<Stream>::operator>>(uint16_t &v)
    {
        deserialize_for_integers(stream, v);
        return *this;
    }

    template <typename Stream>
    EndianSafeBinaryStream<Stream> &EndianSafeBinaryStream<Stream>::operator<<(const int32_t &v)
    {
        Helper::serialize_for_integers(stream, v);
        return *this;
    }

    template <typename Stream> EndianSafeBinaryStream<Stream> &EndianSafeBinaryStream<Stream>::operator>>(int32_t &v)
    {
        Helper::deserialize_for_integers(stream, v);
        return *this;
    }

    template <typename Stream>
    EndianSafeBinaryStream<Stream> &EndianSafeBinaryStream<Stream>::operator<<(const uint32_t &v)
    {
        Helper::serialize_for_integers(stream, v);
        return *this;
    }

    template <typename Stream> EndianSafeBinaryStream<Stream> &EndianSafeBinaryStream<Stream>::operator>>(uint32_t &v)
    {
        Helper::deserialize_for_integers(stream, v);
        return *this;
    }

    template <typename Stream>
    EndianSafeBinaryStream<Stream> &EndianSafeBinaryStream<Stream>::operator<<(const int64_t &v)
    {
        Helper::serialize_for_integers(stream, v);
        return *this;
    }

    template <typename Stream> EndianSafeBinaryStream<Stream> &EndianSafeBinaryStream<Stream>::operator>>(int64_t &v)
    {
        Helper::deserialize_for_integers(stream, v);
        return *this;
    }

    template <typename Stream>
    EndianSafeBinaryStream<Stream> &EndianSafeBinaryStream<Stream>::operator<<(const uint64_t &v)
    {
        Helper::serialize_for_integers(stream, v);
        return *this;
    }

    template <typename Stream> EndianSafeBinaryStream<Stream> &EndianSafeBinaryStream<Stream>::operator>>(uint64_t &v)
    {
        Helper::deserialize_for_integers(stream, v);
        return *this;
    }

    template <typename Stream>
    EndianSafeBinaryStream<Stream> &EndianSafeBinaryStream<Stream>::operator<<(const float &v)
    {
        Helper::serialize_for_floating_point(stream, v);
        return *this;
    }

    template <typename Stream> EndianSafeBinaryStream<Stream> &EndianSafeBinaryStream<Stream>::operator>>(float &v)
    {
        Helper::deserialize_for_floating_point(stream, v);
        return *this;
    }

    template <typename Stream>
    EndianSafeBinaryStream<Stream> &EndianSafeBinaryStream<Stream>::operator<<(const double &v)
    {
        Helper::serialize_for_floating_point(stream, v);
        return *this;
    }

    template <typename Stream> EndianSafeBinaryStream<Stream> &EndianSafeBinaryStream<Stream>::operator>>(double &v)
    {
        Helper::deserialize_for_floating_point(stream, v);
        return *this;
    }

    template <typename Stream>
    EndianSafeBinaryStream<Stream> &EndianSafeBinaryStream<Stream>::write_array(const uint8_t *v, size_t length)
    {
        for (size_t i = 0; i < length; i += array_buffer_size)
        {
            size_t length_to_write = std::min(array_buffer_size, length - i);
            stream.write(reinterpret_cast<const char *>(v + i), length_to_write);
        }
        return *this;
    }

    template <typename Stream>
    EndianSafeBinaryStream<Stream> &EndianSafeBinaryStream<Stream>::read_array(uint8_t *v, size_t length)
    {
        for (size_t i = 0; i < length; i += array_buffer_size)
        {
            size_t length_to_read = std::min(array_buffer_size, length - i);
            stream.read(reinterpret_cast<char *>(v + i), length_to_read);
        }
        return *this;
    }

} // namespace esbs
