#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <core/fluidSolver/particleCollection/CompactParticleCollection.hpp>
#include <core/fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchGreedyAllocated.hpp>
#include <core/fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchDynamicAllocated.hpp>
#include <core/fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchPreAllocated.hpp>
#include <core/fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch.hpp>
#include <core/fluidSolver/neighborhoodSearch/CompactHashingNeighborhoodSearch.hpp>

/**
 * Generates points from start (including) to end(including) with step distance as a grid in both axis directions.
 * The points are ordered by x axis first then y axis, for example:
 * \verbatim
 * 0  1  2  3  4  5
 * 6  7  8  9  10 11
 * 12 13 14 15 16 17
 * @endverbatim
 * @param start
 * @param step
 * @param stop
 * @return
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

std::vector<FluidSolver::FluidParticle> TurnPositionsIntoParticles(const std::vector<glm::vec2> &positions) {
    std::vector<FluidSolver::FluidParticle> ret;
    ret.reserve(positions.size());
    for (auto pos : positions) {
        FluidSolver::FluidParticle particle;
        particle.Position = pos;
        particle.Type = FluidSolver::ParticleType::ParticleTypeNormal;
        ret.push_back(particle);
    }
    return ret;
}

bool IsParticleIndexInsideNeighborhood(FluidSolver::Neighbors *val, FluidSolver::particleIndex_t search) {
    auto begin = val->begin();
    auto end = val->end();
    for (auto current = begin; current != end; current++) {
        if ((*current) == search)
            return true;
    }
    return false;
}


template<typename T>
class NeighborhoodSearchTest : public ::testing::Test {

};

TYPED_TEST_SUITE_P(NeighborhoodSearchTest);


TYPED_TEST_P(NeighborhoodSearchTest, ShouldWorkForSingleParticle) {

    auto particleCollection = new FluidSolver::CompactParticleCollection();
    const float radius = 2.0f;

    // add a single particle
    FluidSolver::FluidParticle particle;
    particle.Position = glm::vec2(234.56341f, 6578.45f);
    particleCollection->AddParticle(particle);

    TypeParam container(particleCollection, radius);
    container.FindNeighbors();

    FluidSolver::NeighborsCompact neighbors = container.GetNeighbors(0);


    EXPECT_EQ(1, neighbors.size());
    EXPECT_EQ(0, *neighbors.begin());

    delete particleCollection;
}

TYPED_TEST_P(NeighborhoodSearchTest, UniformSampledRegion) {
    auto particleCollection = new FluidSolver::CompactParticleCollection();
    const float radius = 2.0f;

    // set up uniform sampled region
    auto part = TurnPositionsIntoParticles(GetSampleGrid(-5, 1, 5));
    particleCollection->AddParticles(part);

    // set up neighborhood search
    TypeParam container(particleCollection, radius);
    container.FindNeighbors();

    // Neighbors of particle zero
    FluidSolver::NeighborsCompact neighborsParticle0 = container.GetNeighbors(0);
    EXPECT_EQ(6, neighborsParticle0.size());
    EXPECT_TRUE(IsParticleIndexInsideNeighborhood(&neighborsParticle0, 0));
    EXPECT_TRUE(IsParticleIndexInsideNeighborhood(&neighborsParticle0, 1));
    EXPECT_TRUE(IsParticleIndexInsideNeighborhood(&neighborsParticle0, 2));
    EXPECT_TRUE(IsParticleIndexInsideNeighborhood(&neighborsParticle0, 11));
    EXPECT_TRUE(IsParticleIndexInsideNeighborhood(&neighborsParticle0, 12));
    EXPECT_TRUE(IsParticleIndexInsideNeighborhood(&neighborsParticle0, 22));
    EXPECT_FALSE(IsParticleIndexInsideNeighborhood(&neighborsParticle0, 13));
    EXPECT_FALSE(IsParticleIndexInsideNeighborhood(&neighborsParticle0, 23));
    EXPECT_FALSE(IsParticleIndexInsideNeighborhood(&neighborsParticle0, 33));
    EXPECT_FALSE(IsParticleIndexInsideNeighborhood(&neighborsParticle0, 35));
    EXPECT_FALSE(IsParticleIndexInsideNeighborhood(&neighborsParticle0, 3));
    EXPECT_FALSE(IsParticleIndexInsideNeighborhood(&neighborsParticle0, 60));

    // Neighbors of particle 38
    FluidSolver::NeighborsCompact neighborsParticle38 = container.GetNeighbors(38);
    EXPECT_EQ(13, neighborsParticle38.size());
    EXPECT_TRUE(IsParticleIndexInsideNeighborhood(&neighborsParticle38, 16));
    EXPECT_TRUE(IsParticleIndexInsideNeighborhood(&neighborsParticle38, 26));
    EXPECT_TRUE(IsParticleIndexInsideNeighborhood(&neighborsParticle38, 27));
    EXPECT_TRUE(IsParticleIndexInsideNeighborhood(&neighborsParticle38, 28));
    EXPECT_TRUE(IsParticleIndexInsideNeighborhood(&neighborsParticle38, 36));
    EXPECT_TRUE(IsParticleIndexInsideNeighborhood(&neighborsParticle38, 37));
    EXPECT_TRUE(IsParticleIndexInsideNeighborhood(&neighborsParticle38, 38));
    EXPECT_TRUE(IsParticleIndexInsideNeighborhood(&neighborsParticle38, 39));
    EXPECT_TRUE(IsParticleIndexInsideNeighborhood(&neighborsParticle38, 40));
    EXPECT_TRUE(IsParticleIndexInsideNeighborhood(&neighborsParticle38, 48));
    EXPECT_TRUE(IsParticleIndexInsideNeighborhood(&neighborsParticle38, 49));
    EXPECT_TRUE(IsParticleIndexInsideNeighborhood(&neighborsParticle38, 50));
    EXPECT_TRUE(IsParticleIndexInsideNeighborhood(&neighborsParticle38, 60));
    EXPECT_FALSE(IsParticleIndexInsideNeighborhood(&neighborsParticle38, 29));
    EXPECT_FALSE(IsParticleIndexInsideNeighborhood(&neighborsParticle38, 25));
    EXPECT_FALSE(IsParticleIndexInsideNeighborhood(&neighborsParticle38, 47));
    EXPECT_FALSE(IsParticleIndexInsideNeighborhood(&neighborsParticle38, 59));
    EXPECT_FALSE(IsParticleIndexInsideNeighborhood(&neighborsParticle38, 61));
    EXPECT_FALSE(IsParticleIndexInsideNeighborhood(&neighborsParticle38, 71));
    EXPECT_FALSE(IsParticleIndexInsideNeighborhood(&neighborsParticle38, 5));
    EXPECT_FALSE(IsParticleIndexInsideNeighborhood(&neighborsParticle38, 2));

    using ::testing::UnorderedElementsAre;

    EXPECT_THAT(neighborsParticle38, UnorderedElementsAre(16,26,27,28,36,37,38,39,40,48,49,50,60));


    delete particleCollection;
}

REGISTER_TYPED_TEST_SUITE_P(NeighborhoodSearchTest,
                           ShouldWorkForSingleParticle,
                           UniformSampledRegion
);


// placeholder for test bodies
typedef ::testing::Types<
        FluidSolver::QuadraticNeighborhoodSearchGreedyAllocated,
        FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated,
        FluidSolver::QuadraticNeighborhoodSearchPreAllocated,
        FluidSolver::HashedNeighborhoodSearch,
        FluidSolver::CompactHashingNeighborhoodSearch
> NeighborhoodSearchTypes;
INSTANTIATE_TYPED_TEST_SUITE_P(NeighborhoodSearchTypesInstantiation, NeighborhoodSearchTest, NeighborhoodSearchTypes);