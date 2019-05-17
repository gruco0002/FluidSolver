//
// Created by gruberc on 17.05.19.
//

#ifndef FLUIDSOLVER_QUADRATICNEIGHBORHOODSEARCHPREALLOCATED_HPP
#define FLUIDSOLVER_QUADRATICNEIGHBORHOODSEARCHPREALLOCATED_HPP

#include <core/interface/INeighborhoodSearch.hpp>

namespace FluidSolver {
    class QuadraticNeighborhoodSearchPreAllocated : public INeighborhoodSearch {

    public:
        bool SupportsGlobalNeighborhoodFinding() override;

        void FindNeighbors(IParticleCollection *particleCollection, float radius) override;

        void FindNeighbors(uint32_t particleIndex, IParticleCollection *particleCollection, float radius) override;

        Neighbors GetParticleNeighbors(uint32_t particleIndex) override;

    private:

        std::vector<uint32_t> neighbors;
        std::vector<uint32_t> neighborsCount;
        std::vector<uint32_t> neighborsStart;

    };
}

#endif //FLUIDSOLVER_QUADRATICNEIGHBORHOODSEARCHPREALLOCATED_HPP
