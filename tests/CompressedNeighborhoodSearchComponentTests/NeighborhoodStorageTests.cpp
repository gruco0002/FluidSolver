#include "fluidSolver/neighborhoodSearch/CompressedNeighbors.hpp"

#include <gtest/gtest.h>

TEST(CompressedNeighborhoodSearch, NeighborhoodStorage1)
{
    using namespace LibFluid;

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

TEST(CompressedNeighborhoodSearch, NeighborhoodStorageMove)
{
    using namespace LibFluid;

    CompressedNeighborhoodSearch::NeighborStorage storage;
    storage.clear();
    storage.set_first_neighbor(123);
    for (size_t i = 0; i < 19; i++)
    {
        storage.set_next_neighbor(i + 1);
    }

    CompressedNeighborhoodSearch::NeighborStorage moved_by_constructor(std::move(storage));

    ASSERT_EQ(123, moved_by_constructor.get_first_neighbor());
    ASSERT_EQ(20, moved_by_constructor.size());
    for (size_t i = 0; i < 19; i++)
    {
        ASSERT_EQ(i + 1, moved_by_constructor.get_delta(i));
    }

    CompressedNeighborhoodSearch::NeighborStorage moved_by_assignment;
    moved_by_assignment = std::move(moved_by_constructor);

    ASSERT_EQ(123, moved_by_assignment.get_first_neighbor());
    ASSERT_EQ(20, moved_by_assignment.size());
    for (size_t i = 0; i < 19; i++)
    {
        ASSERT_EQ(i + 1, moved_by_assignment.get_delta(i));
    }
}

TEST(CompressedNeighborhoodSearch, NeighborhoodStorageCopy)
{
    using namespace LibFluid;

    CompressedNeighborhoodSearch::NeighborStorage storage;
    storage.clear();
    storage.set_first_neighbor(123);
    for (size_t i = 0; i < 19; i++)
    {
        storage.set_next_neighbor(i + 1);
    }

    CompressedNeighborhoodSearch::NeighborStorage copied_by_constructor(storage);

    ASSERT_EQ(123, copied_by_constructor.get_first_neighbor());
    ASSERT_EQ(20, copied_by_constructor.size());
    for (size_t i = 0; i < 19; i++)
    {
        ASSERT_EQ(i + 1, copied_by_constructor.get_delta(i));
    }

    storage.set_next_neighbor(12);
    ASSERT_EQ(20, copied_by_constructor.size());
    ASSERT_EQ(21, storage.size());

    CompressedNeighborhoodSearch::NeighborStorage copied_by_assignment;
    copied_by_assignment = std::move(copied_by_constructor);

    ASSERT_EQ(123, copied_by_assignment.get_first_neighbor());
    ASSERT_EQ(20, copied_by_assignment.size());
    for (size_t i = 0; i < 19; i++)
    {
        ASSERT_EQ(i + 1, copied_by_assignment.get_delta(i));
    }

    storage.set_next_neighbor(12);
    copied_by_constructor.set_next_neighbor(8);
    ASSERT_EQ(21, copied_by_constructor.size());
    ASSERT_EQ(22, storage.size());
    ASSERT_EQ(20, copied_by_assignment.size());
}
