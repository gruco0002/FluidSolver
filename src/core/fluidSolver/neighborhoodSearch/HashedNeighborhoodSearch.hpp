//
// Created by corbi on 21.05.2019.
//

#ifndef FLUIDSOLVER_HASHEDNEIGHBORHOODSEARCH_HPP
#define FLUIDSOLVER_HASHEDNEIGHBORHOODSEARCH_HPP

#include <core/fluidSolver/neighborhoodSearch/INeighborhoodSearch.hpp>
#include <unordered_map>
#include <vector>
#include <list>

namespace FluidSolver {
	class HashedNeighborhoodSearch : public INeighborhoodSearch {


	private:
		typedef std::pair<int32_t, int32_t> GridKey;


	public:
		void FindNeighbors(IParticleCollection* particleCollection, float radius) override;

		Neighbors GetParticleNeighbors(uint32_t particleIndex) override;

		void SetParticleCount(uint32_t particleCount) override;

		HashedNeighborhoodSearch(float gridCellSize);

	protected:
		void FindNeighbors(uint32_t particleIndex, IParticleCollection* particleCollection, float radius) override;


	private:


		struct GridKeyHash {
			//static_assert(sizeof(int32_t) * 2 == sizeof(size_t));

			size_t operator()(GridKey p) const noexcept {
				return (size_t((uint32_t)p.first) << 32) | ((uint32_t)p.second);
			}
		};

		/**
		 * Can be an arbitrary value, but it is recommended to set it to 3 times the particle size.
		 */
		float gridCellSize;

		/**
		 * Maps the Grid cell to its containing particles
		 */
		std::unordered_map<GridKey, std::list<uint32_t>, GridKeyHash> gridToParticles;

		/**
		 * Contains the current status of each particle.
		 */
		std::unordered_map<uint32_t, GridKey> currentStatus;

		/**
		 * Map containing the neighbor count and neighbors of a given particle
		 */
		std::unordered_map<uint32_t, std::pair<uint32_t, std::vector<uint32_t >>> neighbors;

		uint32_t bucketsCreatedUntilIndex = 0;

		GridKey GetGridCellByParticleID(uint32_t particleIndex, IParticleCollection* particleCollection);

		void CreateGridEntryIfNecessary(GridKey& key);


	};
}

#endif //FLUIDSOLVER_HASHEDNEIGHBORHOODSEARCH_HPP
