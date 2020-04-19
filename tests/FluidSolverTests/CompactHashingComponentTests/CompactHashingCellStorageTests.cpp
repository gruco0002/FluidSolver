#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <core/fluidSolver/neighborhoodSearch/CompactHashingNeighborhoodSearch.hpp>

using ::testing::UnorderedElementsAre;
using ::testing::SizeIs;
using ::testing::Each;
using ::testing::Not;
using ::testing::AnyOf;

TEST(CompactHashingCellStorageTest, Insert) {

    auto storage = FluidSolver::CompactHashingNeighborhoodSearch::CellStorage(12);
    auto storageSection = storage.GetEmptyStorageSection();
    auto gridKey = FluidSolver::CompactHashingNeighborhoodSearch::GridCell(0, 0);


    for (FluidSolver::particleIndex_t index = 0; index < 9; index++) {
        storage.AddParticleToStorageSection(storageSection, index, gridKey);
    }


    EXPECT_EQ(9, storage.GetStorageSectionElementCount(storageSection));
    EXPECT_EQ(gridKey, storage.GetStorageSectionGridCell(storageSection));

    auto begin = storage.GetStorageSectionDataBegin(storageSection);
    auto end = storage.GetStorageSectionDataEnd(storageSection);
    std::vector<FluidSolver::particleIndex_t> data;
    for (auto current = begin; current != end; current++) {
        data.push_back((*current).particleIndex.value);
    }

    EXPECT_EQ(9, data.size());
    EXPECT_THAT(data, UnorderedElementsAre(0, 1, 2, 3, 4, 5, 6, 7, 8));


}

TEST(CompactHashingCellStorageTest, InsertIntoMultipleCells) {

    auto storage = FluidSolver::CompactHashingNeighborhoodSearch::CellStorage(12);
    auto gridKey = FluidSolver::CompactHashingNeighborhoodSearch::GridCell(0, 0);

    auto storageSection1 = storage.GetEmptyStorageSection();
    storage.AddParticleToStorageSection(storageSection1, 0, gridKey);

    auto storageSection2 = storage.GetEmptyStorageSection();
    storage.AddParticleToStorageSection(storageSection2, 0, gridKey);

    auto storageSection3 = storage.GetEmptyStorageSection();
    storage.AddParticleToStorageSection(storageSection3, 0, gridKey);

    std::vector<size_t> storageSections = {storageSection1, storageSection2, storageSection3};


    for (FluidSolver::particleIndex_t index = 1; index < 9; index++) {
        storage.AddParticleToStorageSection(storageSection1, index, gridKey);
        storage.AddParticleToStorageSection(storageSection2, index, gridKey);
        storage.AddParticleToStorageSection(storageSection3, index, gridKey);
    }


    for (auto storageSection : storageSections) {
        EXPECT_EQ(9, storage.GetStorageSectionElementCount(storageSection));
        EXPECT_EQ(gridKey, storage.GetStorageSectionGridCell(storageSection));

        auto begin = storage.GetStorageSectionDataBegin(storageSection);
        auto end = storage.GetStorageSectionDataEnd(storageSection);
        std::vector<FluidSolver::particleIndex_t> data;
        for (auto current = begin; current != end; current++) {
            data.push_back((*current).particleIndex.value);
        }

        EXPECT_EQ(9, data.size());
        EXPECT_THAT(data, UnorderedElementsAre(0, 1, 2, 3, 4, 5, 6, 7, 8));
    }


}

TEST(CompactHashingCellStorageTest, InsertRemove) {

    auto storage = FluidSolver::CompactHashingNeighborhoodSearch::CellStorage(12);
    auto storageSection = storage.GetEmptyStorageSection();
    auto gridKey = FluidSolver::CompactHashingNeighborhoodSearch::GridCell(0, 0);


    for (FluidSolver::particleIndex_t index = 0; index < 9; index++) {
        storage.AddParticleToStorageSection(storageSection, index, gridKey);
    }

    // checking for correct insertion
    EXPECT_EQ(9, storage.GetStorageSectionElementCount(storageSection));
    EXPECT_EQ(gridKey, storage.GetStorageSectionGridCell(storageSection));

    auto begin = storage.GetStorageSectionDataBegin(storageSection);
    auto end = storage.GetStorageSectionDataEnd(storageSection);
    std::vector<FluidSolver::particleIndex_t> data;
    for (auto current = begin; current != end; current++) {
        data.push_back((*current).particleIndex.value);
    }

    EXPECT_EQ(9, data.size());
    EXPECT_THAT(data, UnorderedElementsAre(0, 1, 2, 3, 4, 5, 6, 7, 8));

    // removing two items
    storage.RemoveParticleFromStorageSection(storageSection, 8);
    storage.RemoveParticleFromStorageSection(storageSection, 3);

    // checking new state
    EXPECT_EQ(7, storage.GetStorageSectionElementCount(storageSection));
    EXPECT_EQ(gridKey, storage.GetStorageSectionGridCell(storageSection));

    auto begin2 = storage.GetStorageSectionDataBegin(storageSection);
    auto end2 = storage.GetStorageSectionDataEnd(storageSection);
    std::vector<FluidSolver::particleIndex_t> data2;
    for (auto current = begin2; current != end2; current++) {
        data2.push_back((*current).particleIndex.value);
    }

    EXPECT_EQ(7, data2.size());
    EXPECT_THAT(data2, UnorderedElementsAre(0, 1, 2, 4, 5, 6, 7));


}

TEST(CompactHashingCellStorageTest, RemoveDouble) {

    auto storage = FluidSolver::CompactHashingNeighborhoodSearch::CellStorage(12);
    auto storageSection = storage.GetEmptyStorageSection();
    auto gridKey = FluidSolver::CompactHashingNeighborhoodSearch::GridCell(0, 0);


    for (FluidSolver::particleIndex_t index = 0; index < 9; index++) {
        storage.AddParticleToStorageSection(storageSection, index, gridKey);
    }

    // checking for correct insertion
    EXPECT_EQ(9, storage.GetStorageSectionElementCount(storageSection));
    EXPECT_EQ(gridKey, storage.GetStorageSectionGridCell(storageSection));

    auto begin = storage.GetStorageSectionDataBegin(storageSection);
    auto end = storage.GetStorageSectionDataEnd(storageSection);
    std::vector<FluidSolver::particleIndex_t> data;
    for (auto current = begin; current != end; current++) {
        data.push_back((*current).particleIndex.value);
    }

    EXPECT_EQ(9, data.size());
    EXPECT_THAT(data, UnorderedElementsAre(0, 1, 2, 3, 4, 5, 6, 7, 8));

    // removing two items
    storage.RemoveParticleFromStorageSection(storageSection, 3);
    storage.RemoveParticleFromStorageSection(storageSection, 3);

    // checking new state
    EXPECT_EQ(8, storage.GetStorageSectionElementCount(storageSection));
    EXPECT_EQ(gridKey, storage.GetStorageSectionGridCell(storageSection));

    auto begin2 = storage.GetStorageSectionDataBegin(storageSection);
    auto end2 = storage.GetStorageSectionDataEnd(storageSection);
    std::vector<FluidSolver::particleIndex_t> data2;
    for (auto current = begin2; current != end2; current++) {
        data2.push_back((*current).particleIndex.value);
    }

    EXPECT_EQ(8, data2.size());
    EXPECT_THAT(data2, UnorderedElementsAre(0, 1, 2, 4, 5, 6, 7, 8));


}

TEST(CompactHashingCellStorageTest, RemoveNonExistant) {

    auto storage = FluidSolver::CompactHashingNeighborhoodSearch::CellStorage(12);
    auto storageSection = storage.GetEmptyStorageSection();
    auto gridKey = FluidSolver::CompactHashingNeighborhoodSearch::GridCell(0, 0);


    for (FluidSolver::particleIndex_t index = 0; index < 9; index++) {
        storage.AddParticleToStorageSection(storageSection, index, gridKey);
    }

    // checking for correct insertion
    EXPECT_EQ(9, storage.GetStorageSectionElementCount(storageSection));
    EXPECT_EQ(gridKey, storage.GetStorageSectionGridCell(storageSection));

    auto begin = storage.GetStorageSectionDataBegin(storageSection);
    auto end = storage.GetStorageSectionDataEnd(storageSection);
    std::vector<FluidSolver::particleIndex_t> data;
    for (auto current = begin; current != end; current++) {
        data.push_back((*current).particleIndex.value);
    }

    EXPECT_EQ(9, data.size());
    EXPECT_THAT(data, UnorderedElementsAre(0, 1, 2, 3, 4, 5, 6, 7, 8));

    // removing two items
    storage.RemoveParticleFromStorageSection(storageSection, 34);
    storage.RemoveParticleFromStorageSection(storageSection, 9);

    // checking new state
    EXPECT_EQ(9, storage.GetStorageSectionElementCount(storageSection));
    EXPECT_EQ(gridKey, storage.GetStorageSectionGridCell(storageSection));

    auto begin2 = storage.GetStorageSectionDataBegin(storageSection);
    auto end2 = storage.GetStorageSectionDataEnd(storageSection);
    std::vector<FluidSolver::particleIndex_t> data2;
    for (auto current = begin2; current != end2; current++) {
        data2.push_back((*current).particleIndex.value);
    }

    EXPECT_EQ(9, data2.size());
    EXPECT_THAT(data2, UnorderedElementsAre(0, 1, 2, 3, 4, 5, 6, 7, 8));


}


TEST(CompactHashingCellStorageTest, InsertOverfill) {

    auto storage = FluidSolver::CompactHashingNeighborhoodSearch::CellStorage(6);
    auto storageSection = storage.GetEmptyStorageSection();
    auto gridKey = FluidSolver::CompactHashingNeighborhoodSearch::GridCell(0, 0);


    for (FluidSolver::particleIndex_t index = 0; index < 15; index++) {
        storage.AddParticleToStorageSection(storageSection, index, gridKey);
    }


    ASSERT_EQ(15, storage.GetStorageSectionElementCount(storageSection));
    ASSERT_EQ(gridKey, storage.GetStorageSectionGridCell(storageSection));

    auto begin = storage.GetStorageSectionDataBegin(storageSection);
    auto end = storage.GetStorageSectionDataEnd(storageSection);
    std::vector<FluidSolver::particleIndex_t> data;
    for (auto current = begin; current != end; current++) {
        data.push_back((*current).particleIndex.value);
    }

    EXPECT_EQ(15, data.size());
    EXPECT_THAT(data, UnorderedElementsAre(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14));


}

TEST(CompactHashingCellStorageTest, RemoveMultipleOccurencesOfTheSameValue) {

    auto storage = FluidSolver::CompactHashingNeighborhoodSearch::CellStorage(12);
    auto storageSection = storage.GetEmptyStorageSection();
    auto gridKey = FluidSolver::CompactHashingNeighborhoodSearch::GridCell(0, 0);


    storage.AddParticleToStorageSection(storageSection, 4, gridKey);
    for (FluidSolver::particleIndex_t index = 0; index < 9; index++) {
        storage.AddParticleToStorageSection(storageSection, index, gridKey);
    }
    storage.AddParticleToStorageSection(storageSection, 4, gridKey);

    // checking for correct insertion
    EXPECT_EQ(11, storage.GetStorageSectionElementCount(storageSection));
    EXPECT_EQ(gridKey, storage.GetStorageSectionGridCell(storageSection));

    auto begin = storage.GetStorageSectionDataBegin(storageSection);
    auto end = storage.GetStorageSectionDataEnd(storageSection);
    std::vector<FluidSolver::particleIndex_t> data;
    for (auto current = begin; current != end; current++) {
        data.push_back((*current).particleIndex.value);
    }

    EXPECT_EQ(11, data.size());
    EXPECT_THAT(data, UnorderedElementsAre(0, 1, 2, 3, 4, 5, 6, 7, 8, 4, 4));

    // removing all occurences of 4
    storage.RemoveParticleFromStorageSection(storageSection, 4);

    // checking new state
    EXPECT_EQ(8, storage.GetStorageSectionElementCount(storageSection));
    EXPECT_EQ(gridKey, storage.GetStorageSectionGridCell(storageSection));

    auto begin2 = storage.GetStorageSectionDataBegin(storageSection);
    auto end2 = storage.GetStorageSectionDataEnd(storageSection);
    std::vector<FluidSolver::particleIndex_t> data2;
    for (auto current = begin2; current != end2; current++) {
        data2.push_back((*current).particleIndex.value);
    }

    EXPECT_EQ(8, data2.size());
    EXPECT_THAT(data2, UnorderedElementsAre(0, 1, 2, 3, 5, 6, 7, 8));


}

TEST(CompactHashingCellStorageTest, InsertOverfillRemove) {

    auto storage = FluidSolver::CompactHashingNeighborhoodSearch::CellStorage(6);
    auto storageSection = storage.GetEmptyStorageSection();
    auto gridKey = FluidSolver::CompactHashingNeighborhoodSearch::GridCell(0, 0);


    for (FluidSolver::particleIndex_t index = 0; index < 15; index++) {
        storage.AddParticleToStorageSection(storageSection, index, gridKey);
    }


    ASSERT_EQ(15, storage.GetStorageSectionElementCount(storageSection));
    ASSERT_EQ(gridKey, storage.GetStorageSectionGridCell(storageSection));

    auto begin = storage.GetStorageSectionDataBegin(storageSection);
    auto end = storage.GetStorageSectionDataEnd(storageSection);
    std::vector<FluidSolver::particleIndex_t> data;
    for (auto current = begin; current != end; current++) {
        data.push_back((*current).particleIndex.value);
    }

    EXPECT_EQ(15, data.size());
    EXPECT_THAT(data, UnorderedElementsAre(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14));

    // now remove content
    storage.RemoveParticleFromStorageSection(storageSection, 9);

    ASSERT_EQ(14, storage.GetStorageSectionElementCount(storageSection));
    ASSERT_EQ(gridKey, storage.GetStorageSectionGridCell(storageSection));

    begin = storage.GetStorageSectionDataBegin(storageSection);
    end = storage.GetStorageSectionDataEnd(storageSection);
    data.clear();
    for (auto current = begin; current != end; current++) {
        data.push_back((*current).particleIndex.value);
    }

    EXPECT_EQ(14, data.size());
    EXPECT_THAT(data, UnorderedElementsAre(0, 1, 2, 3, 4, 5, 6, 7, 8, 10, 11, 12, 13, 14));


}

TEST(CompactHashingCellStorageTest, InsertOverfillRemoveOverfill) {

    auto storage = FluidSolver::CompactHashingNeighborhoodSearch::CellStorage(10);
    auto storageSection = storage.GetEmptyStorageSection();
    auto gridKey = FluidSolver::CompactHashingNeighborhoodSearch::GridCell(0, 0);


    for (FluidSolver::particleIndex_t index = 0; index < 15; index++) {
        storage.AddParticleToStorageSection(storageSection, index, gridKey);
    }


    ASSERT_EQ(15, storage.GetStorageSectionElementCount(storageSection));
    ASSERT_EQ(gridKey, storage.GetStorageSectionGridCell(storageSection));

    auto begin = storage.GetStorageSectionDataBegin(storageSection);
    auto end = storage.GetStorageSectionDataEnd(storageSection);
    std::vector<FluidSolver::particleIndex_t> data;
    for (auto current = begin; current != end; current++) {
        data.push_back((*current).particleIndex.value);
    }

    EXPECT_EQ(15, data.size());
    EXPECT_THAT(data, UnorderedElementsAre(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14));

    // now remove content
    storage.RemoveParticleFromStorageSection(storageSection, 9);
    storage.RemoveParticleFromStorageSection(storageSection, 14);
    storage.RemoveParticleFromStorageSection(storageSection, 12);
    storage.RemoveParticleFromStorageSection(storageSection, 5);
    storage.RemoveParticleFromStorageSection(storageSection, 11);
    storage.RemoveParticleFromStorageSection(storageSection, 13);
    storage.RemoveParticleFromStorageSection(storageSection, 12); // double remove should not matter

    ASSERT_EQ(9, storage.GetStorageSectionElementCount(storageSection));
    ASSERT_EQ(gridKey, storage.GetStorageSectionGridCell(storageSection));

    begin = storage.GetStorageSectionDataBegin(storageSection);
    end = storage.GetStorageSectionDataEnd(storageSection);
    data.clear();
    for (auto current = begin; current != end; current++) {
        data.push_back((*current).particleIndex.value);
    }

    EXPECT_EQ(9, data.size());
    EXPECT_THAT(data, UnorderedElementsAre(0, 1, 2, 3, 4, 6, 7, 8, 10));


}