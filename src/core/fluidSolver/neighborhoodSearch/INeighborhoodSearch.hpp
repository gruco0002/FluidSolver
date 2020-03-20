#ifndef FLUIDSOLVER_INEIGHBORHOODSEARCH_HPP
#define FLUIDSOLVER_INEIGHBORHOODSEARCH_HPP

#include <cstdint>
#include <vector>
#include <memory>
#include <core/interface/TypeDefinitions.hpp>

#include "core/fluidSolver/particleCollection/IParticleCollection.hpp"

namespace FluidSolver {

    /**
     * The neighbors container represents a way of accessing a neighborhood list via the Neighbors::NeighborsIterator.
     *
     * The container itself is a abstract class that allows the corresponding neighborhood search implementations to
     * define a suitable way of allowing access to their results.
     *
     * There a only a few virtual methods that need to be implemented: GetPointer(iteratorPosition), GetBegin() and
     * GetEnd(). The GetBegin function returns an iteratorPosition that represents the beginning of the neighbor list.
     * GetEnd however returns an iterator position that represents the end() of the neighbor list pointing behind the
     * last element. These functions can be understood like the default stl begin() and end() functions. The GetPointer
     * function returns a pointer to the corresponding value representing the particle index of a neighbor for the given
     * iterator position. The iterator position itself can be chosen arbitrarily by the implementation. It is just a
     * value used internally for the implemented derived class to identify the iterators current state. However there
     * are some constraints among this value: The amount of elements aka the amount of neighbors in the list has to be
     * equal to the GetEnd iterator position minus the GetBegin iterator position. Between those two values (including
     * GetBegin, excluding GetEnd), every value has to result in a valid pointer if given to the GetPointer function.
     * For positions outside the range, especially the value of GetEnd, the GetPointer function does not have to return
     * valid pointers. However it has to return a pointer and is not allowed to cause exceptions or likewise behaviour.
     *
     * The particle index is currently represented as type T which is mapped as a default size_t type. It is independent
     * from the type chosen for the iterator position, meaning that their equality is just coincidence and can not be
     * taken for granted in future implementations.
     */
    class Neighbors {
    public:
        class NeighborsIterator;

        typedef particleIndex_t T;
        typedef NeighborsIterator iterator;
        typedef ptrdiff_t difference_type;
        typedef size_t size_type;
        typedef T value_type;
        typedef T *pointer;
        typedef T &reference;


    public:
        /**
         * Iterator for the neighbors collection. More information regarding the iterating process or the Neighbors
         * container see FluidSolver::Neighbors
         */
        class NeighborsIterator {
        public:
            typedef size_t iteratorPosition_t;

        private:
            Neighbors *neighbors;

            iteratorPosition_t iteratorPosition;
            pointer currentPointer;

        public:
            NeighborsIterator(Neighbors *nb, iteratorPosition_t iteratorPosition);

            bool operator==(const NeighborsIterator &other) const;

            bool operator!=(const NeighborsIterator &other) const;

            Neighbors::T &operator*();

            NeighborsIterator &operator++();

            const NeighborsIterator operator++(int);
        };

    public:

        iterator begin();

        iterator end();

        size_t size();

    protected:

        /**
         * Returns a pointer to the particle index of a neighbor given the iterator position.
         * @param iteratorPosition Iterator position between the beginning position and the ending position.
         * @return Pointer to the particle index of a neighbor.
         */
        virtual pointer GetPointer(NeighborsIterator::iteratorPosition_t iteratorPosition) = 0;

        /**
         * Returns the ending iterator position (exclusive) for the neighbor list.
         * @return Iterator position
         */
        virtual size_t GetEnd() = 0;

        /**
         * Returns the beginning iterator position (inclusive) for the neighbor list.
         * @return Iterator position
         */
        virtual size_t GetBegin() = 0;


    };


    /**
     * The neighborhood search is responsible for finding the neighboring particles for each particle.
     *
     * Therefore it is responsible for not only finding the neighbors in an efficient way but storing the result in such
     * a way, that fast access to the data is ensured.
     *
     * While initialization the search obtains a particle collection and a search radius. All particles within this
     * radius are considered neighbors of a particle. This definition of neighbors can also be applied to arbitrary
     * positions, that do not represent a particle. Furthermore the definition includes that particles are always
     * neighbors of themselves.
     *
     * The neighborhood search will be executed by the call to the FindNeighbors() function.
     *
     * The results have to be provided by the GetNeighbors(particleIndex) function as an iterable object. Results for
     * finding neighbors of arbitrary points should be returned by GetNeighbors(position) function.
     *
     * It is often not possible to precalculate neighbors for arbitrary positions. Therefore GetNeighbors(position) can
     * be slightly inefficient.
     *
     * As mentioned in the definition of FluidSolver::IParticleCollection the particle indices can change without
     * notice. If the neighborhood search relies on the indices consistency it should consider using the particle ID.
     * However using this approach can result in a performance loss since further data lookups are required. If the case
     * lets you assume that particle indices change only very seldom, it could be easier to determine if the indices
     * have changed and completely regenerate the underlying data structure of the neighborhood search. This could seem
     * to be inefficient, but the additional computation time can amortize itself, since this event would only occur
     * every n steps and would prevent the use of the particle ID. Determining if the indices have changed since the
     * last call is possible due to IParticleCollection::GetIndicesChangedCounter.
     *
     * It is also possible that the size of the particle collection has changed. The neighborhood search must account
     * for that if needed.
     *
     * Neighbors for particles of type FluidSolver::ParticleType::ParticleTypeDead should not be calculated. Accessing
     * the neighbors of a dead particle however has to be safe and must not result in memory access errors or memory
     * corruptions. However the set of neighbors for a particle of such type has to be empty.
     */
    class INeighborhoodSearch {

    public:

        /**
         * Constructs a new instance of a neighborhood search.
         * @param particleCollection The particle collection that will be searched through.
         * @param radius The search radius for the neighborhood search.
         */
        INeighborhoodSearch(IParticleCollection *particleCollection, float radius);

        virtual ~INeighborhoodSearch() = default;

        /**
         * Finds the neighbors of all particles and saves them for fast access later on. The calculated neighbor lists
         * are not updated if the collection or the contained particle data changes after the call of this function.
         * An updated an thus correct neighborhood lists after such a change can only be obtained by calling this
         * function again.
         */
        virtual void FindNeighbors() = 0;


        /**
         * Returns the already found and saved neighbors of a particle.
         * @param particleIndex Particle index of the particle
         * @return Returns Neighbor object
         */
        virtual std::shared_ptr<Neighbors> GetNeighbors(particleIndex_t particleIndex) = 0;

        /**
         * Returns the neighboring particles for a given position.
         * @note This call can cause the calculation of the neighbors for the given position, since precalculation for
         * arbitrary positions is often not possible. Therefore the use of this function should be reduced.
         * @param position The position
         * @return Neighbor object
         */
        virtual std::shared_ptr<Neighbors> GetNeighbors(glm::vec2 position) = 0;


    protected:

        IParticleCollection *particleCollection = nullptr;

        float radius = 0.0f;


    };


    /**
     * A compact pointer based neighbors collection.
     *
     * This collection points to a compact block of particle indices thar correspond to the meant neighbors. Therefore
     * it is initialized with a pointer to the first neighbor and the total amount of neighbors in this list.
     *
     * @note
     * The collection only acts as a reference to the given data and does not copy nor store it inside itself.
     */
    class NeighborsCompact : public Neighbors {
    public:
        NeighborsCompact(pointer firstNeighbor, size_t neighborCount);

    protected:
        pointer GetPointer(NeighborsIterator::iteratorPosition_t iteratorPosition) override;

        size_t GetEnd() override;

        size_t GetBegin() override;

    private:
        pointer firstNeighbor;
        size_t neighborCount;

    };


}


#endif //FLUIDSOLVER_INEIGHBORHOODSEARCH_HPP
