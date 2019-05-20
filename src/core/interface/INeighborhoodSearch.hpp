//
// Created by corbi on 24.04.2019.
//

#ifndef FLUIDSOLVER_INEIGHBORHOODSEARCH_HPP
#define FLUIDSOLVER_INEIGHBORHOODSEARCH_HPP

#include <cstdint>
#include <vector>

#include "IParticleCollection.hpp"

namespace FluidSolver {

    class NeighborsIterator;

    class Neighbors {
    public:
        typedef uint32_t T;
        typedef NeighborsIterator iterator;
        typedef ptrdiff_t difference_type;
        typedef size_t size_type;
        typedef T value_type;
        typedef T *pointer;
        typedef T &reference;

        Neighbors(T *start, size_t size);

        iterator begin();

        iterator end();

        size_t size();


    private:
        friend class NeighborsIterator;

        size_t _size;
        T *start;


    };

    class NeighborsIterator {
    private:

        size_t position;
        Neighbors &neighbors;
    public:
        NeighborsIterator(Neighbors &nb, size_t position);

        bool operator==(const NeighborsIterator &other) const;

        bool operator!=(const NeighborsIterator &other) const;

        Neighbors::T &operator*();

        NeighborsIterator &operator++();

        const NeighborsIterator operator++(int);
    };

    /**
     * Neighborhood Search interface.
     * @note A particle i is always a neighbor of itself!
     */
    class INeighborhoodSearch {

    public:

        virtual bool SupportsGlobalNeighborhoodFinding();

        virtual void FindNeighbors(IParticleCollection *particleCollection, float radius);

        virtual void FindNeighbors(uint32_t particleIndex, IParticleCollection *particleCollection, float radius) = 0;

        virtual Neighbors GetParticleNeighbors(uint32_t particleIndex) = 0;


    };


}


#endif //FLUIDSOLVER_INEIGHBORHOODSEARCH_HPP
