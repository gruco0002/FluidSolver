#ifndef FLUIDSOLVER_QUADRATICNEIGHBORHOODSEARCH_HPP
#define FLUIDSOLVER_QUADRATICNEIGHBORHOODSEARCH_HPP

#include <vector>
#include <unordered_map>
#include <utility>
#include "INeighborhoodSearch.hpp"

namespace FluidSolver {

    /**
     * A quadratic neighborhood search that allocates storage in a greedy way.
     *
     * The neighbors are stored in an unordered map whose map key corresponds to the particle index. The value of an
     * entry of the unordered map is a tuple consisting of the current neighbor list count and a vector of particle
     * indices. The memory of the vector is never reduced and thus always stays the same or becomes larger.
     * Before the actual neighborhood search is started, an empty entry for each particle is added inside the unordered
     * map if not already existing.
     * The actual search is executed in parallel. For each particle the vector of neighbors is overwritten with the
     * indices of the found neighbors and the counter variable as first part of the tuple is changed to the new amount
     * of neighbors found. The neighbors are found by again iterating over all particles and comparing the distance.
     *
     * @note This neighborhood search is very inefficient and therefore not recommended to be used. However this
     * implementation avoids, in contrast to FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated, new memory
     * allocations.
     */
    class QuadraticNeighborhoodSearchGreedyAllocated : public INeighborhoodSearch {

    public:
        void FindNeighbors() override;

        NeighborsCompact GetNeighbors(particleIndex_t particleIndex) override;

        NeighborsCompactData GetNeighbors(glm::vec2 position) override;

    private:

        /**
         * tuple contains amount of neighbours, vector contains neighbours (vector size can differ from actual amount of
         * neighbours in it)
         */
        std::unordered_map<particleIndex_t, std::pair<particleAmount_t, std::vector<particleIndex_t >>> neighbors;

        size_t bucketsCreatedUntilIndex = 0;

    };
}


#endif //FLUIDSOLVER_QUADRATICNEIGHBORHOODSEARCH_HPP
