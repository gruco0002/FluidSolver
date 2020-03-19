#ifndef FLUIDSOLVER_QUADRATICNEIGHBORHOODSEARCH_HPP
#define FLUIDSOLVER_QUADRATICNEIGHBORHOODSEARCH_HPP

#include <vector>
#include <unordered_map>
#include <utility>
#include "INeighborhoodSearch.hpp"

namespace FluidSolver {
    class QuadraticNeighborhoodSearchGreedyAllocated : public INeighborhoodSearch {

    protected:
        void FindNeighbors(uint32_t particleIndex, IParticleCollection *particleCollection, float radius) override;

    public:
        void SetParticleCount(uint32_t particleCount) override;

        Neighbors GetParticleNeighbors(uint32_t particleIndex) override;


    private:
        // tuple contains count, vector contains neighbors
        std::unordered_map<uint32_t, std::pair<uint32_t, std::vector<uint32_t >>> neighbors;

        uint32_t bucketsCreatedUntilIndex = 0;

    };
}


#endif //FLUIDSOLVER_QUADRATICNEIGHBORHOODSEARCH_HPP
