#pragma once

#include "core/FluidInclude.hpp"
#include "core/fluidSolver/ParticleCollection.hpp"
#include "core/fluidSolver/neighborhoodSearch/NeighborhoodInterface.hpp"

#include <memory>
#include <vector>

namespace FluidSolver
{

    /**
     * @brief Simple quadratic runtime neighborhood search with greedy dynamic memory allocations
     *
     *
     * The runtime of one neighborhood search call is quadratic in the number of particles.
     * The memory to save
     * the particle neighbors is dynamically allocated and managed in a greedy way.
     * This means that storage is
     * never freed or resized to a smaller chunk during the search.
     *
     * This search should mainly be used to
     * verify other searches or basic simualtion behaviour
     * and results, since its implementation is fairly easy
     * and its runtime performance is bad.
     *
     */
    class QuadraticNeighborhoodSearch3D {
      public:
        using particleIndex_t = pIndex_t;
        using particleAmount_t = uint16_t;


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
                vec3 position;
                particleIndex_t particle;
            } of = {};
            bool position_based = false;
            QuadraticNeighborhoodSearch3D* data = nullptr;

            NeighborsIterator begin() const;

            NeighborsIterator end() const;
        };

        std::shared_ptr<ParticleCollection> collection = nullptr;
        pFloat search_radius = 0.0f;

        void find_neighbors();

        Neighbors get_neighbors(particleIndex_t particleIndex);

        Neighbors get_neighbors(const vec3& position);

        void initialize();

        NeighborhoodInterface create_interface();

      private:
        struct NeighborData
        {
            size_t size;
            std::vector<particleIndex_t> neighbor_indices;
        };


        std::vector<NeighborData> neighbor_data;
    };


} // namespace FluidSolver