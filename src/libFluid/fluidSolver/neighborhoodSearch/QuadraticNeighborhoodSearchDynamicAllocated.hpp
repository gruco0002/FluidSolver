#ifndef FLUIDSOLVER_QUADRATICNEIGHBORHOODSEARCHDYNAMICALLOCATED_HPP
#define FLUIDSOLVER_QUADRATICNEIGHBORHOODSEARCHDYNAMICALLOCATED_HPP

#include "NeighborhoodInterface.hpp"
#include "FluidInclude.hpp"
#include "fluidSolver/ParticleCollection.hpp"
#include "Compatibility.hpp"

#include <memory>
#include <unordered_map>
#include <vector>

namespace FluidSolver
{

    /**
     * A quadratic neighborhood search that allocates storage dynamically.
     *
     * The neighbors are
     * stored in an unordered map whose map key corresponds to the particle index. The value of an
     * entry of the
     * unordered map is a vector of particle indices.
     * Before the actual neighborhood search is started, an empty
     * entry is added inside the unordered map if not already
     * existing.
     * The actual search is executed in
     * parallel. For each particle the vector of neighbors is cleared and then refilled
     * by again iterating over
     * all particles and checking the distance between the particles.
     *
     * @note This neighborhood search is
     * very inefficient and therefore not recommended to be used. However this
     * implementation can assist by
     * helping to verify results of more complex implementations of the neighborhood
     * search.
     *
     *
     *
     * * Types:
     *      NeighborsIterator
     *          An iterator that allows to iterate over the neighbors.

     * *          Dereferencing the iterator yields a size_t particle index.
     *      Neighbors
     *          An
     * object that can be queried for an iterator to iterate over the neighbors.
     *          It must therefore
     * support the following functions:
     *          NeighborsIterator begin();
     *          NeighborsIterator
     * end();
     * Fields:
     *      ParticleCollection* collection;
     *      float search_radius;
     *
     * Functions:
     *      void find_neighbors();
     *      Neighbors get_neighbors(size_t particleIndex);
     *
     * Neighbors get_neighbors(const glm::vec2 &position);
     *
     *      NeighborhoodInterface
     * create_interface();
     */
    class QuadraticNeighborhoodSearchDynamicAllocated {
      public:
        using particleIndex_t = pIndex_t;

        struct Neighbors;

        struct NeighborsIterator
        {

            const Neighbors* data;
            particleIndex_t current;

            bool operator==(const NeighborsIterator& other) const;

            bool operator!=(const NeighborsIterator& other) const;

            particleIndex_t& operator*();

            NeighborsIterator& operator++();

            const NeighborsIterator operator++(int);
        };

        struct Neighbors
        {

            // iterator defines
            using T = particleIndex_t;
            using iterator = NeighborsIterator;
            using const_iterator = NeighborsIterator;
            using difference_type = ptrdiff_t;
            using size_type = size_t;
            using value_type = T;
            using pointer = T*;
            using const_pointer = const T*;
            using reference = T&;

            // data
            union {
                vec2 position;
                particleIndex_t particle;
            } of = {};
            bool position_based = false;
            QuadraticNeighborhoodSearchDynamicAllocated* data = nullptr;

            NeighborsIterator begin() const;

            NeighborsIterator end() const;
        };

        std::shared_ptr<ParticleCollection> collection = nullptr;
        float search_radius = 0.0f;

        void find_neighbors();

        Neighbors get_neighbors(particleIndex_t particleIndex);

        Neighbors get_neighbors(const vec2& position);


        void initialize();

        NeighborhoodInterface create_interface();

        Compatibility check();

      private:
        std::unordered_map<particleIndex_t, std::vector<particleIndex_t>> neighbors;
    };


} // namespace FluidSolver


#endif // FLUIDSOLVER_QUADRATICNEIGHBORHOODSEARCHDYNAMICALLOCATED_HPP
