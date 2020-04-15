#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <core/fluidSolver/particleCollection/CompactParticleCollection.hpp>
#include <core/fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchGreedyAllocated.hpp>
#include <core/fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchDynamicAllocated.hpp>
#include <core/fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchPreAllocated.hpp>
#include <core/fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch.hpp>
#include <core/fluidSolver/neighborhoodSearch/CompactHashingNeighborhoodSearch.hpp>

#include <libraries/glm/glm.hpp>
#include <libraries/glm/gtx/matrix_transform_2d.hpp>

using ::testing::UnorderedElementsAre;
using ::testing::SizeIs;
using ::testing::Each;
using ::testing::Not;
using ::testing::AnyOf;

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

std::vector<glm::vec2> TransformVector(const std::vector<glm::vec2> &positions, const glm::mat3 &transformation) {
    std::vector<glm::vec2> result;
    result.reserve(positions.size());
    for (auto pos : positions) {
        glm::vec3 transformed = glm::vec3(pos.x, pos.y, 1.0f);
        result.push_back(glm::vec2(transformed.x, transformed.y));
    }
    return result;
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

template<typename T>
class NeighborhoodSearchTest : public ::testing::Test {

};

TYPED_TEST_SUITE_P(NeighborhoodSearchTest);


TYPED_TEST_P(NeighborhoodSearchTest, ShouldWorkForSingleParticle) {

    auto particleCollection = FluidSolver::CompactParticleCollection();
    const float radius = 2.0f;

    // add a single particle
    FluidSolver::FluidParticle particle;
    particle.Position = glm::vec2(234.56341f, 6578.45f);
    particleCollection.AddParticle(particle);

    TypeParam container(&particleCollection, radius);
    container.FindNeighbors();

    FluidSolver::NeighborsCompact neighbors = container.GetNeighbors(0);


    EXPECT_EQ(1, neighbors.size());
    EXPECT_EQ(0, *neighbors.begin());

}

TYPED_TEST_P(NeighborhoodSearchTest, UniformSampledParticles) {
    auto particleCollection = FluidSolver::CompactParticleCollection();
    const float radius = 2.0f;

    // set up uniform sampled region
    auto part = TurnPositionsIntoParticles(GetSampleGrid(-5, 1, 5));
    particleCollection.AddParticles(part);

    // set up neighborhood search
    TypeParam container(&particleCollection, radius);
    container.FindNeighbors();

    // Neighbors of particle zero
    FluidSolver::NeighborsCompact neighborsParticle0 = container.GetNeighbors(0);
    EXPECT_THAT(neighborsParticle0, SizeIs(6));
    EXPECT_THAT(neighborsParticle0, UnorderedElementsAre(0, 1, 2, 11, 12, 22));
    EXPECT_THAT(neighborsParticle0, Each(Not(AnyOf(13, 23, 33, 35, 3, 60))));

    // Neighbors of particle 38
    FluidSolver::NeighborsCompact neighborsParticle38 = container.GetNeighbors(38);
    EXPECT_THAT(neighborsParticle38, SizeIs(13));
    EXPECT_THAT(neighborsParticle38, UnorderedElementsAre(16, 26, 27, 28, 36, 37, 38, 39, 40, 48, 49, 50, 60));
    EXPECT_THAT(neighborsParticle38, Each(Not(AnyOf(29, 25, 47, 59, 61, 71, 5, 2))));

}

TYPED_TEST_P(NeighborhoodSearchTest, UniformSampledParticlesScaled) {
    auto particleCollection = FluidSolver::CompactParticleCollection();
    const float radius = 2.0f * 2.0f;

    // set up uniform sampled region
    auto part = TurnPositionsIntoParticles(GetSampleGrid(-5 * 2.0f, 1 * 2.0f, 5 * 2.0f));
    particleCollection.AddParticles(part);

    // set up neighborhood search
    TypeParam container(&particleCollection, radius);
    container.FindNeighbors();

    // Neighbors of particle zero
    FluidSolver::NeighborsCompact neighborsParticle0 = container.GetNeighbors(0);
    EXPECT_THAT(neighborsParticle0, SizeIs(6));
    EXPECT_THAT(neighborsParticle0, UnorderedElementsAre(0, 1, 2, 11, 12, 22));
    EXPECT_THAT(neighborsParticle0, Each(Not(AnyOf(13, 23, 33, 35, 3, 60))));

    // Neighbors of particle 38
    FluidSolver::NeighborsCompact neighborsParticle38 = container.GetNeighbors(38);
    EXPECT_THAT(neighborsParticle38, SizeIs(13));
    EXPECT_THAT(neighborsParticle38, UnorderedElementsAre(16, 26, 27, 28, 36, 37, 38, 39, 40, 48, 49, 50, 60));
    EXPECT_THAT(neighborsParticle38, Each(Not(AnyOf(29, 25, 47, 59, 61, 71, 5, 2))));

}

TYPED_TEST_P(NeighborhoodSearchTest, UniformSampledParticlesRotated) {
    auto particleCollection = FluidSolver::CompactParticleCollection();
    const float radius = 2.0f;

    // set up uniform sampled region
    auto part = TurnPositionsIntoParticles(
            TransformVector(GetSampleGrid(-5, 1, 5), glm::rotate(glm::mat3(1.0f), 37.4f)));
    particleCollection.AddParticles(part);

    // set up neighborhood search
    TypeParam container(&particleCollection, radius);
    container.FindNeighbors();

    // Neighbors of particle zero
    FluidSolver::NeighborsCompact neighborsParticle0 = container.GetNeighbors(0);
    EXPECT_THAT(neighborsParticle0, SizeIs(6));
    EXPECT_THAT(neighborsParticle0, UnorderedElementsAre(0, 1, 2, 11, 12, 22));
    EXPECT_THAT(neighborsParticle0, Each(Not(AnyOf(13, 23, 33, 35, 3, 60))));

    // Neighbors of particle 38
    FluidSolver::NeighborsCompact neighborsParticle38 = container.GetNeighbors(38);
    EXPECT_THAT(neighborsParticle38, SizeIs(13));
    EXPECT_THAT(neighborsParticle38, UnorderedElementsAre(16, 26, 27, 28, 36, 37, 38, 39, 40, 48, 49, 50, 60));
    EXPECT_THAT(neighborsParticle38, Each(Not(AnyOf(29, 25, 47, 59, 61, 71, 5, 2))));

}


REGISTER_TYPED_TEST_SUITE_P(NeighborhoodSearchTest,
                            ShouldWorkForSingleParticle,
                            UniformSampledParticles,
                            UniformSampledParticlesScaled,
                            UniformSampledParticlesRotated
);


// placeholder for test bodies
typedef ::testing::Types<
        FluidSolver::QuadraticNeighborhoodSearchGreedyAllocated,
        FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated,
        FluidSolver::QuadraticNeighborhoodSearchPreAllocated,
        FluidSolver::HashedNeighborhoodSearch, FluidSolver::CompactHashingNeighborhoodSearch
> NeighborhoodSearchTypes;
INSTANTIATE_TYPED_TEST_SUITE_P(NeighborhoodSearchTypesInstantiation, NeighborhoodSearchTest, NeighborhoodSearchTypes);