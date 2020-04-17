#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <core/fluidSolver/neighborhoodSearch/CompactHashingNeighborhoodSearch.hpp>

using ::testing::UnorderedElementsAre;
using ::testing::SizeIs;
using ::testing::Each;
using ::testing::Not;
using ::testing::AnyOf;

TEST(CompactHashingCellStorageTest, SimpleInsertTest) {

    auto storage = FluidSolver::CompactHashingNeighborhoodSearch::CellStorage(6);
    auto storageSection = storage.GetEmptyStorageSection();
    auto gridKey = FluidSolver::CompactHashingNeighborhoodSearch::GridCell(0,0);


    for(FluidSolver::particleIndex_t index = 0; index < 20; index++){
        storage.AddParticleToStorageSection(storageSection, index, gridKey);
    }


    EXPECT_EQ(20, storage.GetStorageSectionElementCount(storageSection));
    EXPECT_EQ(gridKey, storage.GetStorageSectionGridCell(storageSection));

    auto begin = storage.GetStorageSectionDataBegin(storageSection);
    auto end = storage.GetStorageSectionDataEnd(storageSection);
    std::vector<FluidSolver::particleIndex_t> data;
    for(auto current = begin; current != end; current++ ){
        data.push_back((*current).particleIndex.value);
    }

    EXPECT_EQ(20, data.size());
    EXPECT_THAT(data, UnorderedElementsAre(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19));


}