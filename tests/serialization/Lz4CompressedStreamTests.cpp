#include "serialization/Lz4CompressedStream.hpp"

#include <filesystem>
#include <vector>

#include <gtest/gtest.h>
#include <gmock/gmock.h>


TEST(Lz4CompressedStreamTests, TestSmallData) {
    const char* data = "Hello World :)";
    const size_t data_length = strlen(data) + 1;

    const char* filename = "Lz4CompressedStreamTests.TestSmallData.bin";

    // write data
    {
        LibFluid::Lz4CompressedStream stream = LibFluid::Lz4CompressedStream::output(filename);
        stream.write(data, data_length);
    }

    // read data
    char read_in[data_length];
    {
        LibFluid::Lz4CompressedStream stream = LibFluid::Lz4CompressedStream::input(filename);
        stream.read(read_in, data_length);
    }

    // delete temporary file
    try {
        std::filesystem::remove(filename);
    } catch (const std::exception& e) {
        EXPECT_TRUE(false) << "Could not cleanup temporary files: " << e.what();
    }

    // test equality
    ASSERT_STREQ(data, read_in);
}

TEST(Lz4CompressedStreamTests, TestLargeData) {
    constexpr size_t data_length = 10485760;

    // generate the data
    std::vector<char> data;
    data.resize(data_length);
    for(size_t i = 0; i < data_length; i++)
    {
        data[i] = (char)(i % 127);
    }


    const char* filename = "Lz4CompressedStreamTests.TestLargeData.bin";

    // write data
    {
        LibFluid::Lz4CompressedStream stream = LibFluid::Lz4CompressedStream::output(filename);
        stream.write(data.data(), data.size());
    }

    // read data
    std::vector<char> read_in;
    read_in.resize(data_length);
    {
        LibFluid::Lz4CompressedStream stream = LibFluid::Lz4CompressedStream::input(filename);
        stream.read(read_in.data(), data_length);
    }

    // delete temporary file
    try {
        std::filesystem::remove(filename);
    } catch (const std::exception& e) {
        EXPECT_TRUE(false) << "Could not cleanup temporary files: " << e.what();
    }

    // test equality
    ASSERT_THAT(read_in, ::testing::ContainerEq(data));
}

TEST(Lz4CompressedStreamTests, TestReadOver) {
    const char* data = "Hello World :)";
    const size_t data_length = strlen(data) + 1;

    const char* filename = "Lz4CompressedStreamTests.TestReadOver.bin";

    // write data
    {
        LibFluid::Lz4CompressedStream stream = LibFluid::Lz4CompressedStream::output(filename);
        stream.write(data, data_length);
    }

    // read data
    char read_in[data_length + 5];
    {
        LibFluid::Lz4CompressedStream stream = LibFluid::Lz4CompressedStream::input(filename);
        stream.read(read_in, data_length + 5);
        EXPECT_TRUE(!stream);
    }

    // delete temporary file
    try {
        std::filesystem::remove(filename);
    } catch (const std::exception& e) {
        EXPECT_TRUE(false) << "Could not cleanup temporary files: " << e.what();
    }

    // test equality
    ASSERT_STREQ(data, read_in);
}

TEST(Lz4CompressedStreamTests, TestReadAfterDone) {
    const char* data = "Hello World :)";
    const size_t data_length = strlen(data) + 1;

    const char* filename = "Lz4CompressedStreamTests.TestReadAfterDone.bin";

    // write data
    {
        LibFluid::Lz4CompressedStream stream = LibFluid::Lz4CompressedStream::output(filename);
        stream.write(data, data_length);
    }

    // read data
    char read_in[data_length];
    {
        LibFluid::Lz4CompressedStream stream = LibFluid::Lz4CompressedStream::input(filename);
        stream.read(read_in, data_length);
        EXPECT_FALSE(!stream);

        char tmp[10];
        stream.read(tmp, 10);
        EXPECT_TRUE(!stream);
    }

    // delete temporary file
    try {
        std::filesystem::remove(filename);
    } catch (const std::exception& e) {
        EXPECT_TRUE(false) << "Could not cleanup temporary files: " << e.what();
    }

    // test equality
    ASSERT_STREQ(data, read_in);
}