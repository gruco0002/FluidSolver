#include "fluidSolver/neighborhoodSearch/CompressedNeighbors.hpp"

#include <gtest/gtest.h>

TEST(CompressedNeighborhoodSearch, NeighborhoodStorage1)
{
    using namespace FluidSolver;

    CompressedNeighborhoodSearch::NeighborStorage storage;
    storage.clear();

    ASSERT_EQ(0, storage.size());

    storage.set_first_neighbor(123);
    ASSERT_EQ(123, storage.get_first_neighbor());

    ASSERT_EQ(1, storage.size());

    for (size_t i = 0; i < 19; i++)
    {
        storage.set_next_neighbor(i + 1);
    }

    ASSERT_EQ(20, storage.size());

    for (size_t i = 0; i < 19; i++)
    {
        ASSERT_EQ(i + 1, storage.get_delta(i));
    }
}