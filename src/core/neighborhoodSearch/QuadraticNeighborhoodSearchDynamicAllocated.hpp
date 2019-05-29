//
// Created by gruberc on 20.05.19.
//

#ifndef FLUIDSOLVER_QUADRATICNEIGHBORHOODSEARCHDYNAMICALLOCATED_HPP
#define FLUIDSOLVER_QUADRATICNEIGHBORHOODSEARCHDYNAMICALLOCATED_HPP

#include <core/interface/IParticleCollection.hpp>
#include <core/interface/INeighborhoodSearch.hpp>
#include <unordered_map>

namespace FluidSolver {
    class QuadraticNeighborhoodSearchDynamicAllocated : public INeighborhoodSearch {
    protected:
        void FindNeighbors(uint32_t particleIndex, IParticleCollection *particleCollection, float radius) override;

    public:

        Neighbors GetParticleNeighbors(uint32_t particleIndex) override;

        void SetParticleCount(uint32_t particleCount) override;


    private:
        // tuple contains count, vector contains neighbors
        std::unordered_map<uint32_t, std::vector<uint32_t >> neighbors;


    };


}


#endif //FLUIDSOLVER_QUADRATICNEIGHBORHOODSEARCHDYNAMICALLOCATED_HPP