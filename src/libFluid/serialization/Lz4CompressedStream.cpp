#include "Lz4CompressedStream.hpp"
#include <FluidAssert.hpp>


namespace FluidSolver {

    Lz4CompressedStream::Lz4CompressedStream(const std::filesystem::path& filepath, std::ios_base::openmode mode) {
        if (mode & std::ios_base::in) {
            access_mode = AccessMode::ReadOnly;
            stream = std::fstream(filepath, std::ios_base::binary | std::ios_base::in);
        } else if (mode & std::ios_base::out) {
            access_mode = AccessMode::WriteOnly;
            stream = std::fstream(filepath, std::ios_base::binary | std::ios_base::out);
        }
        initialize();
    }

    Lz4CompressedStream Lz4CompressedStream::input(const std::filesystem::path& filepath) {
        return Lz4CompressedStream(filepath, std::ios_base::in);
    }

    Lz4CompressedStream Lz4CompressedStream::output(const std::filesystem::path& filepath) {
        return Lz4CompressedStream(filepath, std::ios_base::out);
    }

    void Lz4CompressedStream::write(const char* data, size_t length) {
        FLUID_ASSERT(this->compression_context != nullptr);

        if (length == 0) {
            return;
        }

        // determine the maximum size of the compressed data and allocated memory if required
        size_t compressed_data_max_size = LZ4F_compressBound(length, nullptr);
        if (compressed_data_max_size > write_heap_buffer.size()) {
            write_heap_buffer.resize(compressed_data_max_size);
        }

        // compress the data
        size_t compressed_data_size = LZ4F_compressUpdate(this->compression_context, write_heap_buffer.data(), compressed_data_max_size, data, length, nullptr);
        if (LZ4F_isError(compressed_data_size)) {
            throw std::runtime_error("compression failed");
        } else if (compressed_data_size > 0) {
            // there was data written out
            stream.write(write_heap_buffer.data(), compressed_data_size);
        }
    }

    void Lz4CompressedStream::read(char* data, size_t length) {
        FLUID_ASSERT(this->decompression_context != nullptr);

        size_t to_copy = length;
        size_t copied_so_far = 0;
        while (to_copy > 0) {
            if (read_decompressed_buffer.has_data()) {
                // the buffer contains data -> copy it over to data

                // determine the amount to copy
                size_t copied_amount = std::min(to_copy, read_decompressed_buffer.size());

                // copy the data
                std::memcpy(data + copied_so_far, read_decompressed_buffer.data(), copied_amount);

                // change the counters
                read_decompressed_buffer.consume(copied_amount);
                to_copy -= copied_amount;
                copied_so_far += copied_amount;
            } else {
                // the buffer is empty decompress the next batch
                if (read_source_buffer.has_data()) {
                    // the source buffer has still data available, consume it first
                    size_t destination_size = Buffer::max_size;
                    size_t source_size = read_source_buffer.size();

                    LZ4F_decompressOptions_t options = {};
                    options.stableDst = 0;

                    size_t result = LZ4F_decompress(this->decompression_context,
                            read_decompressed_buffer.new_data(), &destination_size,
                            read_source_buffer.data(), &source_size,
                            &options);

                    if (LZ4F_isError(result)) {
                        throw std::runtime_error("could not decompress data");
                    }

                    // consume the used up bytes from the source buffer
                    read_source_buffer.consume(source_size);
                    read_decompressed_buffer.set_new_size(destination_size);

                } else {
                    // the source buffer is empty, read in more data
                    stream.read(read_source_buffer.new_data(), FluidSolver::Lz4CompressedStream::Buffer::max_size);
                    size_t read_bytes = stream.gcount();

                    if (read_bytes == 0) {
                        read_reached_eof = true;
                        return;
                    }
                    read_source_buffer.set_new_size(read_bytes);
                }
            }
        }
    }

    void Lz4CompressedStream::close() {
        finalize();
        stream.close();
    }
    Lz4CompressedStream::~Lz4CompressedStream() {
        finalize();
    }
    void Lz4CompressedStream::initialize() {
        if (access_mode == AccessMode::WriteOnly) {
            initialize_write();
        } else {
            initialize_read();
        }
    }
    void Lz4CompressedStream::finalize() {
        if (is_finalized) {
            return;
        }
        is_finalized = true;
        if (access_mode == AccessMode::WriteOnly) {
            finalize_write();
        } else {
            finalize_read();
        }
    }
    void Lz4CompressedStream::initialize_read() {
        LZ4F_errorCode_t error_code = LZ4F_createDecompressionContext(&this->decompression_context, LZ4F_getVersion());
        if (LZ4F_isError(error_code)) {
            throw std::runtime_error("could not create decompression context");
        }
    }
    void Lz4CompressedStream::finalize_read() {
        LZ4F_errorCode_t error_code = LZ4F_freeDecompressionContext(this->decompression_context);
        this->decompression_context = nullptr;
        if (LZ4F_isError(error_code)) {
            throw std::runtime_error("could not free decompression context");
        }
    }

    void Lz4CompressedStream::initialize_write() {
        // create the context
        LZ4F_errorCode_t error_code = LZ4F_createCompressionContext(&this->compression_context, LZ4F_getVersion());
        if (LZ4F_isError(error_code)) {
            throw std::runtime_error("could not create compression context");
        }

        // write the begin header
        char header_data[LZ4F_HEADER_SIZE_MAX] = {};
        size_t header_size = LZ4F_compressBegin(this->compression_context,
                header_data, LZ4F_HEADER_SIZE_MAX,
                nullptr);

        if (LZ4F_isError(header_size)) {
            throw std::runtime_error("could not create compression header");
        }

        stream.write(header_data, header_size);
    }

    void Lz4CompressedStream::finalize_write() {
        // write the end (checksum, ...)

        // first determine the required size for the destination buffer
        size_t max_size = LZ4F_compressBound(0, nullptr);

        // create the end
        std::vector<char> end_data(max_size);
        size_t end_size = LZ4F_compressEnd(this->compression_context, end_data.data(), end_data.size(), nullptr);
        if (LZ4F_isError(end_size)) {
            throw std::runtime_error("could not end compression");
        }

        // write the end to the file
        stream.write(end_data.data(), end_size);

        // free the context
        LZ4F_errorCode_t error_code = LZ4F_freeCompressionContext(this->compression_context);
        this->compression_context = nullptr;
        if (LZ4F_isError(error_code)) {
            throw std::runtime_error("could not free compression context");
        }
    }
    bool Lz4CompressedStream::operator!() const {
        return this->read_reached_eof;
    }

    bool Lz4CompressedStream::Buffer::has_data() const {
        return current_index < current_size;
    }
    size_t Lz4CompressedStream::Buffer::size() const {
        return current_size - current_index;
    }
    char* Lz4CompressedStream::Buffer::data() {
        return current_data + current_index;
    }
    void Lz4CompressedStream::Buffer::consume(size_t size) {
        current_index += size;
    }
    void Lz4CompressedStream::Buffer::set_new_size(size_t new_size) {
        FLUID_ASSERT(new_size <= max_size);
        current_index = 0;
        current_size = new_size;
    }
    char* Lz4CompressedStream::Buffer::new_data() {
        return current_data;
    }

} // namespace FluidSolver
