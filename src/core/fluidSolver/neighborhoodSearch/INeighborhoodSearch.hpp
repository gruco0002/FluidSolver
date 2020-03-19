#ifndef FLUIDSOLVER_INEIGHBORHOODSEARCH_HPP
#define FLUIDSOLVER_INEIGHBORHOODSEARCH_HPP

#include <cstdint>
#include <vector>

#include "core/fluidSolver/particleCollection/IParticleCollection.hpp"

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


        /**
         * Global particle search function: Searches neighbors of all particles
         * @param particleCollection The particle collection
         * @param radius Radius of the search
         * @note This function will not be called in parallel.
         */
        virtual void FindNeighbors(IParticleCollection *particleCollection, float radius);

        virtual ~INeighborhoodSearch() = default;

    protected:
        /**
         * Finds the neighbors to a single particle and saves them
         * @param particleIndex The index of the particle
         * @param particleCollection The particle collection
         * @param radius Radius of the search
         * @note This function will automatically be called by FindNeighbors() in parallel if not changed by
         *       implementation.
         * @note This function will/could be called in parallel for multiple particles
         */
        virtual void FindNeighbors(uint32_t particleIndex, IParticleCollection *particleCollection, float radius) = 0;

    public:
        /**
         * Returns the already found and saved neighbors of a particle
         * @param particleIndex Particle index of the particle
         * @return Returns the already found and saved neighbors of a particle
         */
        virtual Neighbors GetParticleNeighbors(uint32_t particleIndex) = 0;

        /**
         * Will be called before starting the neighborhood search.
         * @param particleCount The amount of particles exisiting
         */
        virtual void SetParticleCount(uint32_t particleCount);


    };


}


#endif //FLUIDSOLVER_INEIGHBORHOODSEARCH_HPP
