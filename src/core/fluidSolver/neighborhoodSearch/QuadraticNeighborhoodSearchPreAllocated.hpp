#ifndef FLUIDSOLVER_QUADRATICNEIGHBORHOODSEARCHPREALLOCATED_HPP
#define FLUIDSOLVER_QUADRATICNEIGHBORHOODSEARCHPREALLOCATED_HPP

#include <core/fluidSolver/neighborhoodSearch/INeighborhoodSearch.hpp>

namespace FluidSolver {

    /**
     * A quadratic neighborhood search that allocates storage before saving the neighbors.
     *
     * The neighbors are stored in on continuous compact array without empty array cells. Furthermore the structure
     * contains two arrays mapping the particle indices to either the start position in the large neighbor array or the
     * amount of neighbors.
     * The search behaves the following way: First of all the neighbors are searched by comparing all particles with
     * each other and counted. These numbers are accumulated to reserve fixed ranges in the neighbor array and in order
     * to resize the neighbor array if needed. Then a second search is executed which now saves the found neighbor
     * indices inside the reserved array ranges.
     *
     * @note This neighborhood search is very inefficient and therefore not recommended to be used. However this
     * implementation avoids, in contrast to FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated, new memory
     * allocations.
     */
    class QuadraticNeighborhoodSearchPreAllocated : public INeighborhoodSearch {

    public:
        QuadraticNeighborhoodSearchPreAllocated(IParticleCollection *particleCollection, float radius);

        void FindNeighbors() override;

        NeighborsCompact GetNeighbors(particleIndex_t particleIndex) override;

        NeighborsCompactData GetNeighbors(glm::vec2 position) override;

    private:

        std::vector<particleIndex_t> neighbors;
        std::vector<particleAmount_t> neighborsCount;
        std::vector<particleAmount_t> neighborsStart;

    };
}

#endif //FLUIDSOLVER_QUADRATICNEIGHBORHOODSEARCHPREALLOCATED_HPP
