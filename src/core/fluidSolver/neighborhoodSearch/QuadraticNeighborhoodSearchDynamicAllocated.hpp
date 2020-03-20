#ifndef FLUIDSOLVER_QUADRATICNEIGHBORHOODSEARCHDYNAMICALLOCATED_HPP
#define FLUIDSOLVER_QUADRATICNEIGHBORHOODSEARCHDYNAMICALLOCATED_HPP

#include <core/fluidSolver/particleCollection/IParticleCollection.hpp>
#include <core/fluidSolver/neighborhoodSearch/INeighborhoodSearch.hpp>
#include <unordered_map>

namespace FluidSolver {

    /**
     * A quadratic neighborhood search that allocates storage dynamically.
     *
     * The neighbors are stored in an unordered map whose map key corresponds to the particle index. The value of an
     * entry of the unordered map is a vector of particle indices.
     * Before the actual neighborhood search is started, an empty entry is added inside the unordered map if not already
     * existing.
     * The actual search is executed in parallel. For each particle the vector of neighbors is cleared and then refilled
     * by again iterating over all particles and checking the distance between the particles.
     *
     * @note This neighborhood search is very inefficient and therefore not recommended to be used. However this
     * implementation can assist by helping to verify results of more complex implementations of the neighborhood
     * search.
     */
    class QuadraticNeighborhoodSearchDynamicAllocated : public INeighborhoodSearch {

    public:
        void FindNeighbors() override;

        std::shared_ptr<Neighbors> GetNeighbors(particleIndex_t particleIndex) override;

        std::shared_ptr<Neighbors> GetNeighbors(glm::vec2 position) override;


    private:
        std::unordered_map<particleIndex_t, std::vector<particleIndex_t >> neighbors;

    };


}


#endif //FLUIDSOLVER_QUADRATICNEIGHBORHOODSEARCHDYNAMICALLOCATED_HPP
