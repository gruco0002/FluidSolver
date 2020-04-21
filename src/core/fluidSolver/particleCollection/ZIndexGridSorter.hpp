#ifndef FLUIDSOLVER_ZINDEXGRIDSORTER_HPP
#define FLUIDSOLVER_ZINDEXGRIDSORTER_HPP

#include "IParticleCollection.hpp"

namespace FluidSolver {
    class ZIndexGridSorter {

    public:
        float GridCellSize;

        ZIndexGridSorter(float gridCellSize, IParticleCollection *particleCollection);

        void CalculateGridCoordinates(particleIndex_t particleIndex, int32_t &gridX, int32_t &gridY);

        void CalculateGridCoordinatesUnsigned(particleIndex_t particleIndex, uint32_t &gridX, uint32_t &gridY);

        uint64_t CalculateSortKey(particleIndex_t particleIndex);


        /**
         * Uses merge sort.
         */
        void SortInitially();

        /**
         * Uses insertion sort.
         */
        void Sort();

    private:

        IParticleCollection *particleCollection = nullptr;

    };
}


#endif //FLUIDSOLVER_ZINDEXGRIDSORTER_HPP
