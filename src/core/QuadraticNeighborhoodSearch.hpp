//
// Created by corbi on 24.04.2019.
//

#ifndef FLUIDSOLVER_QUADRATICNEIGHBORHOODSEARCH_HPP
#define FLUIDSOLVER_QUADRATICNEIGHBORHOODSEARCH_HPP

#include <vector>
#include <unordered_map>
#include <utility>
#include "core/interface/INeighborhoodSearch.hpp"

namespace FluidSolver {
    class QuadraticNeighborhoodSearch :public INeighborhoodSearch{

    public:
        void FindNeighbors(uint32_t particleIndex, IParticleCollection *particleCollection, float radius) override;

        Neighbors GetParticleNeighbors(uint32_t particleIndex) override;


    private:
        // tuple contains count, vector contains neighbors
        std::unordered_map<uint32_t, std::pair<uint32_t ,std::vector<uint32_t >>> neighbors;


    };
}


#endif //FLUIDSOLVER_QUADRATICNEIGHBORHOODSEARCH_HPP
