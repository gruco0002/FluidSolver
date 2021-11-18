#include <gtest/gtest.h>

#include "serialization/Lz4CompressedStream.hpp"

TEST(Lz4CompressedStreamTests, TestSmallData)
{
    const char* data = "Hello World :)";
    const size_t data_length = strlen(data) + 1;

    const char* filename = "Lz4CompressedStreamTests.TestSmallData.bin";

    // write data
    {
        FluidSolver::Lz4CompressedStream stream = FluidSolver::Lz4CompressedStream::output(filename);
        stream.write(data, data_length);
    }

    // read data
    {
        FluidSolver::Lz4CompressedStream stream = FluidSolver::Lz4CompressedStream::input(filename);
        char read_in[data_length];
        stream.read(read_in, data_length);

        ASSERT_STREQ(data, read_in);
    }

}