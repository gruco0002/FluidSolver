//
// Created by Corbinian Gruber on 2019-04-29.
//

#include <core/SimpleParticleCollection.hpp>
#include <core/QuadraticNeighborhoodSearch.hpp>
#include "gtest/gtest.h"

/**
 * Creates a grid of positions starting from start with step size in both axis directions.
 * @param start Inclusive start position.
 * @param step Step size of the positions.
 * @param stop Maximum for the grid value in each axis. Is inclusive if step is exactly stop
 * @return
 * @note Grid is sorted from left to right, then top to bottom.
 */
std::vector<glm::vec2> GetSampleGrid(float start, float step, float stop) {

    std::vector<glm::vec2> pos;

    for (float y = start; y <= stop; y += step) {
        for (float x = start; x <= stop; x += step) {
            pos.push_back(glm::vec2(x, y));
        }
    }


    return pos;
}

bool Exists(std::vector<uint32_t> &val, uint32_t search){
    return std::find(val.begin(), val.end(), search) != val.end();
}


TEST(QuadraticNeighborhoodTest, NeighborhoodCount1) {

    float radius = 1.5f;
    auto positions = GetSampleGrid(-2.0f, 1.0f, 2.0f);


    std::vector<FluidSolver::SimpleParticleCollection::FluidParticle> particles;
    for (auto pos : positions) {
        FluidSolver::SimpleParticleCollection::FluidParticle p;
        p.Position = pos;
        particles.push_back(p);
    }
    FluidSolver::QuadraticNeighborhoodSearch neighborhoodSearch;
    FluidSolver::SimpleParticleCollection collection(particles);
    for (uint32_t i = 0; i < collection.GetSize(); i++) {
        neighborhoodSearch.FindNeighbors(i, &collection, radius);
    }


    ASSERT_EQ(neighborhoodSearch.GetParticleNeighbors(0).size(), 4); // top left
    ASSERT_EQ(neighborhoodSearch.GetParticleNeighbors(20).size(), 4); // bottom left
    ASSERT_EQ(neighborhoodSearch.GetParticleNeighbors(12).size(), 9); // center
    ASSERT_EQ(neighborhoodSearch.GetParticleNeighbors(18).size(), 9); // center bottom right
    ASSERT_EQ(neighborhoodSearch.GetParticleNeighbors(10).size(), 6); // border left center
    ASSERT_EQ(neighborhoodSearch.GetParticleNeighbors(14).size(), 6); // border right center

}

TEST(QuadraticNeighborhoodTest, NeighborhoodCount2) {

    float radius = 2.5f;
    auto positions = GetSampleGrid(-2.0f, 1.0f, 2.0f);


    std::vector<FluidSolver::SimpleParticleCollection::FluidParticle> particles;
    for (auto pos : positions) {
        FluidSolver::SimpleParticleCollection::FluidParticle p;
        p.Position = pos;
        particles.push_back(p);
    }
    FluidSolver::QuadraticNeighborhoodSearch neighborhoodSearch;
    FluidSolver::SimpleParticleCollection collection(particles);
    for (uint32_t i = 0; i < collection.GetSize(); i++) {
        neighborhoodSearch.FindNeighbors(i, &collection, radius);
    }


    ASSERT_EQ(neighborhoodSearch.GetParticleNeighbors(0).size(), 8); // top left
    ASSERT_EQ(neighborhoodSearch.GetParticleNeighbors(20).size(), 8); // bottom left
    ASSERT_EQ(neighborhoodSearch.GetParticleNeighbors(12).size(), 21); // center
    ASSERT_EQ(neighborhoodSearch.GetParticleNeighbors(18).size(), 15); // center bottom right
    ASSERT_EQ(neighborhoodSearch.GetParticleNeighbors(10).size(), 13); // border left center
    ASSERT_EQ(neighborhoodSearch.GetParticleNeighbors(14).size(), 13); // border right center


}

TEST(QuadraticNeighborhoodTest, NeighborhoodCorrect1) {

    float radius = 1.5f;
    auto positions = GetSampleGrid(-2.0f, 1.0f, 2.0f);


    std::vector<FluidSolver::SimpleParticleCollection::FluidParticle> particles;
    for (auto pos : positions) {
        FluidSolver::SimpleParticleCollection::FluidParticle p;
        p.Position = pos;
        particles.push_back(p);
    }
    FluidSolver::QuadraticNeighborhoodSearch neighborhoodSearch;
    FluidSolver::SimpleParticleCollection collection(particles);
    for (uint32_t i = 0; i < collection.GetSize(); i++) {
        neighborhoodSearch.FindNeighbors(i, &collection, radius);
    }



    ASSERT_TRUE(Exists(neighborhoodSearch.GetParticleNeighbors(0), 0)); // top left
    ASSERT_TRUE(Exists(neighborhoodSearch.GetParticleNeighbors(0), 1));
    ASSERT_TRUE(Exists(neighborhoodSearch.GetParticleNeighbors(0), 5));
    ASSERT_TRUE(Exists(neighborhoodSearch.GetParticleNeighbors(0), 6));
    ASSERT_EQ(Exists(neighborhoodSearch.GetParticleNeighbors(0), 2), false);
    ASSERT_EQ(Exists(neighborhoodSearch.GetParticleNeighbors(0), 24), false);


    ASSERT_TRUE(Exists(neighborhoodSearch.GetParticleNeighbors(20), 20)); // bottom left
    ASSERT_TRUE(Exists(neighborhoodSearch.GetParticleNeighbors(20), 16));
    ASSERT_EQ(Exists(neighborhoodSearch.GetParticleNeighbors(20), 7), false);

    ASSERT_TRUE(Exists(neighborhoodSearch.GetParticleNeighbors(12), 12)); // center
    ASSERT_TRUE(Exists(neighborhoodSearch.GetParticleNeighbors(12), 7));
    ASSERT_TRUE(Exists(neighborhoodSearch.GetParticleNeighbors(12), 6));
    ASSERT_TRUE(Exists(neighborhoodSearch.GetParticleNeighbors(12), 13));
    ASSERT_EQ(Exists(neighborhoodSearch.GetParticleNeighbors(12), 2), false);
    ASSERT_EQ(Exists(neighborhoodSearch.GetParticleNeighbors(12), 20), false);
    ASSERT_EQ(Exists(neighborhoodSearch.GetParticleNeighbors(12), 22), false);
    ASSERT_EQ(Exists(neighborhoodSearch.GetParticleNeighbors(12), 0), false);
    ASSERT_EQ(Exists(neighborhoodSearch.GetParticleNeighbors(12), 19), false);


}

TEST(QuadraticNeighborhoodTest, NeighborhoodCorrect2) {

    float radius = 2.5f;
    auto positions = GetSampleGrid(-2.0f, 1.0f, 2.0f);


    std::vector<FluidSolver::SimpleParticleCollection::FluidParticle> particles;
    for (auto pos : positions) {
        FluidSolver::SimpleParticleCollection::FluidParticle p;
        p.Position = pos;
        particles.push_back(p);
    }
    FluidSolver::QuadraticNeighborhoodSearch neighborhoodSearch;
    FluidSolver::SimpleParticleCollection collection(particles);
    for (uint32_t i = 0; i < collection.GetSize(); i++) {
        neighborhoodSearch.FindNeighbors(i, &collection, radius);
    }



    ASSERT_TRUE(Exists(neighborhoodSearch.GetParticleNeighbors(0), 0)); // top left
    ASSERT_TRUE(Exists(neighborhoodSearch.GetParticleNeighbors(0), 1));
    ASSERT_TRUE(Exists(neighborhoodSearch.GetParticleNeighbors(0), 5));
    ASSERT_TRUE(Exists(neighborhoodSearch.GetParticleNeighbors(0), 6));
    ASSERT_TRUE(Exists(neighborhoodSearch.GetParticleNeighbors(0), 2));
    ASSERT_TRUE(Exists(neighborhoodSearch.GetParticleNeighbors(0), 11));
    ASSERT_TRUE(Exists(neighborhoodSearch.GetParticleNeighbors(0), 7));
    ASSERT_EQ(Exists(neighborhoodSearch.GetParticleNeighbors(0), 24), false);
    ASSERT_EQ(Exists(neighborhoodSearch.GetParticleNeighbors(0), 12), false);
    ASSERT_EQ(Exists(neighborhoodSearch.GetParticleNeighbors(0), 3), false);


    ASSERT_TRUE(Exists(neighborhoodSearch.GetParticleNeighbors(20), 20)); // bottom left
    ASSERT_TRUE(Exists(neighborhoodSearch.GetParticleNeighbors(20), 16));
    ASSERT_TRUE(Exists(neighborhoodSearch.GetParticleNeighbors(20), 17));
    ASSERT_TRUE(Exists(neighborhoodSearch.GetParticleNeighbors(20), 22));
    ASSERT_EQ(Exists(neighborhoodSearch.GetParticleNeighbors(20), 7), false);
    ASSERT_EQ(Exists(neighborhoodSearch.GetParticleNeighbors(20), 23), false);

    ASSERT_TRUE(Exists(neighborhoodSearch.GetParticleNeighbors(12), 12)); // center
    ASSERT_TRUE(Exists(neighborhoodSearch.GetParticleNeighbors(12), 7));
    ASSERT_TRUE(Exists(neighborhoodSearch.GetParticleNeighbors(12), 6));
    ASSERT_TRUE(Exists(neighborhoodSearch.GetParticleNeighbors(12), 13));
    ASSERT_TRUE(Exists(neighborhoodSearch.GetParticleNeighbors(12), 10));
    ASSERT_TRUE(Exists(neighborhoodSearch.GetParticleNeighbors(12), 3));
    ASSERT_EQ(Exists(neighborhoodSearch.GetParticleNeighbors(12), 0), false);
    ASSERT_EQ(Exists(neighborhoodSearch.GetParticleNeighbors(12), 20), false);
    ASSERT_EQ(Exists(neighborhoodSearch.GetParticleNeighbors(12), 24), false);
    ASSERT_EQ(Exists(neighborhoodSearch.GetParticleNeighbors(12), 4), false);


}