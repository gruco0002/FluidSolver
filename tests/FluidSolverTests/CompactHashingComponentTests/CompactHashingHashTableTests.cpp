#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <core/fluidSolver/neighborhoodSearch/CompactHashingNeighborhoodSearch.hpp>

using ::testing::UnorderedElementsAre;
using ::testing::SizeIs;
using ::testing::Each;
using ::testing::Not;
using ::testing::AnyOf;

TEST(CompactHashingHashTableTest, Insert) {

    auto hashTable = FluidSolver::CompactHashingNeighborhoodSearch::HashTable(25);
    auto gridKey = FluidSolver::CompactHashingNeighborhoodSearch::GridCell(0, 0);
    auto mappedTo = 12;

    hashTable.SetValueByKey(gridKey, mappedTo);

    FluidSolver::CompactHashingNeighborhoodSearch::HashTable::mappedTo_t res;
    bool foundKey = hashTable.GetValueByKey(gridKey, res);
    ASSERT_TRUE(foundKey);
    ASSERT_EQ(mappedTo, res);

}

TEST(CompactHashingHashTableTest, ReadNonExistant) {

    auto hashTable = FluidSolver::CompactHashingNeighborhoodSearch::HashTable(25);
    auto gridKey = FluidSolver::CompactHashingNeighborhoodSearch::GridCell(0, 0);
    auto gridKey2 = FluidSolver::CompactHashingNeighborhoodSearch::GridCell(1, 0);
    auto mappedTo = 12;

    hashTable.SetValueByKey(gridKey, mappedTo);

    FluidSolver::CompactHashingNeighborhoodSearch::HashTable::mappedTo_t res;
    bool foundKey = hashTable.GetValueByKey(gridKey2, res);
    ASSERT_FALSE(foundKey);

}

TEST(CompactHashingHashTableTest, InsertMultiple) {

    auto hashTable = FluidSolver::CompactHashingNeighborhoodSearch::HashTable(25);
    auto gridKey = FluidSolver::CompactHashingNeighborhoodSearch::GridCell(0, 0);
    auto gridKey2 = FluidSolver::CompactHashingNeighborhoodSearch::GridCell(1, 0);

    hashTable.SetValueByKey(gridKey, 42);
    hashTable.SetValueByKey(gridKey2, 23);

    FluidSolver::CompactHashingNeighborhoodSearch::HashTable::mappedTo_t res;
    bool foundKey = hashTable.GetValueByKey(gridKey2, res);
    ASSERT_TRUE(foundKey);
    ASSERT_EQ(23, res);


    foundKey = hashTable.GetValueByKey(gridKey, res);
    ASSERT_TRUE(foundKey);
    ASSERT_EQ(42, res);


}

TEST(CompactHashingHashTableTest, IterateEmpty) {

    auto hashTable = FluidSolver::CompactHashingNeighborhoodSearch::HashTable(25);


    auto begin = hashTable.begin();
    auto end = hashTable.end();

    ASSERT_EQ(begin, end);

}

TEST(CompactHashingHashTableTest, IterateSingleElement) {

    auto hashTable = FluidSolver::CompactHashingNeighborhoodSearch::HashTable(25);
    auto gridKey = FluidSolver::CompactHashingNeighborhoodSearch::GridCell(0, 0);

    hashTable.SetValueByKey(gridKey, 42);

    auto begin = hashTable.begin();
    auto end = hashTable.end();

    ASSERT_NE(begin, end);
    ASSERT_EQ(++begin, end);

    for (auto current = begin; current != end; current++) {
        auto val = *current;
        ASSERT_EQ(gridKey, val);
    }

}

TEST(CompactHashingHashTableTest, IterateMultipleElements) {

    auto hashTable = FluidSolver::CompactHashingNeighborhoodSearch::HashTable(25);
    auto gridKey1 = FluidSolver::CompactHashingNeighborhoodSearch::GridCell(0, 0);
    auto gridKey2 = FluidSolver::CompactHashingNeighborhoodSearch::GridCell(1, -2);
    auto gridKey3 = FluidSolver::CompactHashingNeighborhoodSearch::GridCell(1, 56);
    auto gridKey4 = FluidSolver::CompactHashingNeighborhoodSearch::GridCell(-51, -2);

    hashTable.SetValueByKey(gridKey1, 42);
    hashTable.SetValueByKey(gridKey2, 45);
    hashTable.SetValueByKey(gridKey3, 54);
    hashTable.SetValueByKey(gridKey4, 13);

    auto begin = hashTable.begin();
    auto end = hashTable.end();

    ASSERT_NE(begin, end);

    std::vector<FluidSolver::CompactHashingNeighborhoodSearch::GridCell> keys;
    for (auto current = begin; current != end; current++) {
        auto val = *current;
        keys.push_back(val);
    }

    ASSERT_EQ(4, keys.size());
    ASSERT_THAT(keys, UnorderedElementsAre(gridKey1, gridKey2, gridKey3, gridKey4));

}

TEST(CompactHashingHashTableTest, InsertOverride) {

    auto hashTable = FluidSolver::CompactHashingNeighborhoodSearch::HashTable(25);
    auto gridKey = FluidSolver::CompactHashingNeighborhoodSearch::GridCell(0, 0);

    hashTable.SetValueByKey(gridKey, 12);

    FluidSolver::CompactHashingNeighborhoodSearch::HashTable::mappedTo_t res;
    bool foundKey = hashTable.GetValueByKey(gridKey, res);
    ASSERT_TRUE(foundKey);
    ASSERT_EQ(12, res);

    hashTable.SetValueByKey(gridKey, 23);

    foundKey = hashTable.GetValueByKey(gridKey, res);
    ASSERT_TRUE(foundKey);
    ASSERT_EQ(23, res);

}

TEST(CompactHashingHashTableTest, InsertRemove) {

    auto hashTable = FluidSolver::CompactHashingNeighborhoodSearch::HashTable(25);
    auto gridKey = FluidSolver::CompactHashingNeighborhoodSearch::GridCell(0, 0);

    hashTable.SetValueByKey(gridKey, 12);

    FluidSolver::CompactHashingNeighborhoodSearch::HashTable::mappedTo_t res;
    bool foundKey = hashTable.GetValueByKey(gridKey, res);
    ASSERT_TRUE(foundKey);
    ASSERT_EQ(12, res);

    hashTable.RemoveKey(gridKey);
    foundKey = hashTable.GetValueByKey(gridKey, res);
    ASSERT_FALSE(foundKey);

    // the whole table should be empty now
    auto begin = hashTable.begin();
    auto end = hashTable.end();
    ASSERT_EQ(begin, end);

}

TEST(CompactHashingHashTableTest, InsertMultipleRemoveMultiple) {

    auto hashTable = FluidSolver::CompactHashingNeighborhoodSearch::HashTable(25);
    auto gridKey1 = FluidSolver::CompactHashingNeighborhoodSearch::GridCell(0, 0);
    auto gridKey2 = FluidSolver::CompactHashingNeighborhoodSearch::GridCell(1, -2);
    auto gridKey3 = FluidSolver::CompactHashingNeighborhoodSearch::GridCell(1, 56);
    auto gridKey4 = FluidSolver::CompactHashingNeighborhoodSearch::GridCell(-51, -2);

    hashTable.SetValueByKey(gridKey1, 42);
    hashTable.SetValueByKey(gridKey2, 45);
    hashTable.SetValueByKey(gridKey3, 54);
    hashTable.SetValueByKey(gridKey4, 13);

    hashTable.RemoveKey(gridKey3);


    hashTable.RemoveKey(gridKey2);

    auto begin = hashTable.begin();
    auto end = hashTable.end();
    ASSERT_NE(begin, end);
    std::vector<FluidSolver::CompactHashingNeighborhoodSearch::GridCell> keys;
    for (auto current = begin; current != end; current++) {
        auto val = *current;
        keys.push_back(val);
    }

    ASSERT_EQ(2, keys.size());
    ASSERT_THAT(keys, UnorderedElementsAre(gridKey1, gridKey4));


    hashTable.RemoveKey(gridKey1);
    hashTable.RemoveKey(gridKey4);

    // the whole table should be empty now
    begin = hashTable.begin();
    end = hashTable.end();
    ASSERT_EQ(begin, end);

}

