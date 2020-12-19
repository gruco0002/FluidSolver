#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <core/fluidSolver/ParticleCollection.hpp>

#include <core/fluidSolver/neighborhoodSearch/QuadraticNeighborhoodSearchDynamicAllocated.hpp>
#include <core/fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch.hpp>

#include <glm/glm.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

using ::testing::UnorderedElementsAre;

using ::testing::Each;
using ::testing::Not;
using ::testing::AnyOf;



// define a matcher
MATCHER_P(BeginEndSizeIs, n, "") { size_t counter = 0; for (auto it = arg.begin(); it != arg.end(); it++) counter++; return counter == n; }


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

std::vector<glm::vec2> TransformVector(const std::vector<glm::vec2>& positions, const glm::mat3& transformation) {
	std::vector<glm::vec2> result;
	result.reserve(positions.size());
	for (auto pos : positions) {
		glm::vec3 transformed = glm::vec3(pos.x, pos.y, 1.0f);
		result.push_back(glm::vec2(transformed.x, transformed.y));
	}
	return result;
}

void add_positions(FluidSolver::ParticleCollection& collection, const std::vector<glm::vec2>& positions) {
	for (auto pos : positions) {
		auto index = collection.add();
		collection.get<FluidSolver::MovementData>(index).position = pos;
		collection.get<FluidSolver::ParticleInfo>(index).type = FluidSolver::ParticleType::ParticleTypeNormal;
		collection.get<FluidSolver::ParticleInfo>(index).tag = collection.size() - 1;
	}
}

void setup_collection(FluidSolver::ParticleCollection& collection) {
	collection.add_type<FluidSolver::MovementData>();
	collection.add_type<FluidSolver::ParticleInfo>();
}

template<typename T>
class NeighborhoodSearchTest : public ::testing::Test {

};

TYPED_TEST_SUITE_P(NeighborhoodSearchTest);


TYPED_TEST_P(NeighborhoodSearchTest, ShouldWorkForSingleParticle) {

	auto particleCollection = FluidSolver::ParticleCollection();
	setup_collection(particleCollection);
	const float radius = 2.0f;

	// add a single particle
	add_positions(particleCollection, { glm::vec2(234.56341f, 6578.45f) });


	TypeParam search;
	search.search_radius = radius;
	search.collection = &particleCollection;
	search.initialize();

	// start the neighborhood search
	search.find_neighbors();


	auto neighbors = search.get_neighbors(0);


	EXPECT_THAT(neighbors, BeginEndSizeIs(1));
	EXPECT_EQ(0, *neighbors.begin());

}

TYPED_TEST_P(NeighborhoodSearchTest, UniformSampledParticles) {
	auto particleCollection = FluidSolver::ParticleCollection();
	setup_collection(particleCollection);
	const float radius = 2.0f;

	// set up uniform sampled region
	add_positions(particleCollection, GetSampleGrid(-5, 1, 5));

	// set up neighborhood search
	TypeParam search;
	search.search_radius = radius;
	search.collection = &particleCollection;
	search.initialize();

	search.find_neighbors();

	// Neighbors of particle zero
	auto neighborsParticle0 = search.get_neighbors(0);
	EXPECT_THAT(neighborsParticle0, BeginEndSizeIs(6));
	EXPECT_THAT(neighborsParticle0, UnorderedElementsAre(0, 1, 2, 11, 12, 22));
	EXPECT_THAT(neighborsParticle0, Each(Not(AnyOf(13, 23, 33, 35, 3, 60))));

	// Neighbors of particle 38
	auto neighborsParticle38 = search.get_neighbors(38);
	EXPECT_THAT(neighborsParticle38, BeginEndSizeIs(13));
	EXPECT_THAT(neighborsParticle38, UnorderedElementsAre(16, 26, 27, 28, 36, 37, 38, 39, 40, 48, 49, 50, 60));
	EXPECT_THAT(neighborsParticle38, Each(Not(AnyOf(29, 25, 47, 59, 61, 71, 5, 2))));

}

TYPED_TEST_P(NeighborhoodSearchTest, UniformSampledParticlesScaled) {
	auto particleCollection = FluidSolver::ParticleCollection();
	setup_collection(particleCollection);
	const float radius = 2.0f * 2.0f;

	// set up uniform sampled region
	add_positions(particleCollection, GetSampleGrid(-5 * 2.0f, 1 * 2.0f, 5 * 2.0f));

	// set up neighborhood search
	TypeParam search;
	search.search_radius = radius;
	search.collection = &particleCollection;
	search.initialize();

	search.find_neighbors();

	// Neighbors of particle zero
	auto neighborsParticle0 = search.get_neighbors(0);
	EXPECT_THAT(neighborsParticle0, BeginEndSizeIs(6));
	EXPECT_THAT(neighborsParticle0, UnorderedElementsAre(0, 1, 2, 11, 12, 22));
	EXPECT_THAT(neighborsParticle0, Each(Not(AnyOf(13, 23, 33, 35, 3, 60))));

	// Neighbors of particle 38
	auto neighborsParticle38 = search.get_neighbors(38);
	EXPECT_THAT(neighborsParticle38, BeginEndSizeIs(13));
	EXPECT_THAT(neighborsParticle38, UnorderedElementsAre(16, 26, 27, 28, 36, 37, 38, 39, 40, 48, 49, 50, 60));
	EXPECT_THAT(neighborsParticle38, Each(Not(AnyOf(29, 25, 47, 59, 61, 71, 5, 2))));

}

TYPED_TEST_P(NeighborhoodSearchTest, UniformSampledParticlesRotated) {
	auto particleCollection = FluidSolver::ParticleCollection();
	setup_collection(particleCollection);
	const float radius = 2.0f;

	// set up uniform sampled region
	add_positions(particleCollection, TransformVector(GetSampleGrid(-5, 1, 5), glm::rotate(glm::mat3(1.0f), 37.4f)));

	// set up neighborhood search
	TypeParam search;
	search.search_radius = radius;
	search.collection = &particleCollection;
	search.initialize();

	search.find_neighbors();

	// Neighbors of particle zero
	auto neighborsParticle0 = search.get_neighbors(0);
	EXPECT_THAT(neighborsParticle0, BeginEndSizeIs(6));
	EXPECT_THAT(neighborsParticle0, UnorderedElementsAre(0, 1, 2, 11, 12, 22));
	EXPECT_THAT(neighborsParticle0, Each(Not(AnyOf(13, 23, 33, 35, 3, 60))));

	// Neighbors of particle 38
	auto neighborsParticle38 = search.get_neighbors(38);
	EXPECT_THAT(neighborsParticle38, BeginEndSizeIs(13));
	EXPECT_THAT(neighborsParticle38, UnorderedElementsAre(16, 26, 27, 28, 36, 37, 38, 39, 40, 48, 49, 50, 60));
	EXPECT_THAT(neighborsParticle38, Each(Not(AnyOf(29, 25, 47, 59, 61, 71, 5, 2))));

}

TYPED_TEST_P(NeighborhoodSearchTest, UniformSampledParticlesMoving) {
	auto particleCollection = FluidSolver::ParticleCollection();
	setup_collection(particleCollection);

	const float radius = 2.01f; // since the particles are perfectly distributed and move perfectly along, floating
	// point precision problems cause particles exactly on the border of the neighborhood to
	// fall out, hence the radius is selected a little bit larger, to allow some error margin


	// set up uniform sampled region
	add_positions(particleCollection, GetSampleGrid(-5, 1, 5));

	// set up neighborhood search
	TypeParam search;
	search.search_radius = radius;
	search.collection = &particleCollection;
	search.initialize();


	glm::vec2 totalMovement = glm::vec2(0.0f);
	for (size_t i = 0; i < 1024; i++) {
		const glm::vec2 movement = glm::vec2(0.34f, -0.5f);

		// move the particles
		for (size_t index = 0; index < particleCollection.size(); index++) {
			particleCollection.get<FluidSolver::MovementData>(index).position += movement;
		}
		totalMovement += movement;

		std::string currentState = "Failed after moving the particles " + std::to_string(i + 1) +
			" times.\nThe total distance the particles were moved is { x=" +
			std::to_string(totalMovement.x) + ", y=" + std::to_string(totalMovement.y) + " }.";

		// Execute the neighborhood search
		search.find_neighbors();

		// Neighbors of particle zero
		auto neighborsParticle0 = search.get_neighbors(0);
		ASSERT_THAT(neighborsParticle0, BeginEndSizeIs(6)) << currentState;
		ASSERT_THAT(neighborsParticle0, UnorderedElementsAre(0, 1, 2, 11, 12, 22)) << currentState;
		ASSERT_THAT(neighborsParticle0, Each(Not(AnyOf(13, 23, 33, 35, 3, 60)))) << currentState;

		// Neighbors of particle 38
		auto neighborsParticle38 = search.get_neighbors(38);
		ASSERT_THAT(neighborsParticle38, BeginEndSizeIs(13)) << currentState;
		ASSERT_THAT(neighborsParticle38, UnorderedElementsAre(16, 26, 27, 28, 36, 37, 38, 39, 40, 48, 49, 50, 60))
			<< currentState;
		ASSERT_THAT(neighborsParticle38, Each(Not(AnyOf(29, 25, 47, 59, 61, 71, 5, 2)))) << currentState;
	}

}


REGISTER_TYPED_TEST_SUITE_P(NeighborhoodSearchTest,
	ShouldWorkForSingleParticle,
	UniformSampledParticles,
	UniformSampledParticlesScaled,
	UniformSampledParticlesRotated,
	UniformSampledParticlesMoving
);


// placeholder for test bodies
typedef ::testing::Types<
	FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated,
	FluidSolver::HashedNeighborhoodSearch
> NeighborhoodSearchTypes;
INSTANTIATE_TYPED_TEST_SUITE_P(NeighborhoodSearchTypesInstantiation, NeighborhoodSearchTest, NeighborhoodSearchTypes);