//
// Created by corbi on 24.04.2019.
//

#ifndef FLUIDSOLVER_INEIGHBORHOODSEARCH_HPP
#define FLUIDSOLVER_INEIGHBORHOODSEARCH_HPP

#include <cstdint>
#include <vector>

#include "IParticleCollection.hpp"

namespace FluidSolver {

    /**
     * Neighborhood Search interface.
     * @note A particle i is always a neighbor of itself!
     */
    class INeighborhoodSearch {

    public:

        virtual bool SupportsGlobalNeighborhoodFinding();

        virtual void FindNeighbors(IParticleCollection *particleCollection, float radius);

        virtual void FindNeighbors(uint32_t particleIndex, IParticleCollection *particleCollection, float radius) = 0;

        virtual std::vector<uint32_t > &GetParticleNeighbors(uint32_t particleIndex) = 0;


    };


}


#endif //FLUIDSOLVER_INEIGHBORHOODSEARCH_HPP
