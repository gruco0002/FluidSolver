#pragma once

#include <filesystem>
#include <fstream>
#include <vector>

namespace LibFluid
{

    class Lz4CompressedStream
    {
      public:
        static Lz4CompressedStream input(const std::filesystem::path &filepath);
        static Lz4CompressedStream output(const std::filesystem::path &filepath);

        void write(const char *data, size_t length);
        void read(char *data, size_t length);

        void close();

        bool operator!() const;

        ~Lz4CompressedStream();

        Lz4CompressedStream(Lz4CompressedStream &&);

      private:
        enum class AccessMode
        {
            ReadOnly,
            WriteOnly
        } access_mode = AccessMode::ReadOnly;
        std::fstream stream;

        explicit Lz4CompressedStream(const std::filesystem::path &filepath, std::ios_base::openmode mode);

        bool is_finalized = false;

        void initialize();
        void finalize();

      private:
        void initialize_read();
        void finalize_read();

        void *decompression_context = nullptr;
        struct Buffer
        {
            constexpr static size_t max_size = 16384;

            bool has_data() const;
            size_t size() const;
            char *data();
            void consume(size_t size);

            void set_new_size(size_t new_size);
            char *new_data();

            Buffer() = default;
            Buffer(const Buffer &);
            Buffer &operator=(const Buffer &);
            Buffer(Buffer &&) = delete;

          private:
            char current_data[max_size] = {};
            size_t current_size = 0;
            size_t current_index = 0;
        };

        Buffer read_decompressed_buffer;
        Buffer read_source_buffer;

        bool read_reached_eof = false;

      private:
        void initialize_write();
        void finalize_write();

        void *compression_context = nullptr;
        std::vector<char> write_heap_buffer;
    };

} // namespace LibFluid
