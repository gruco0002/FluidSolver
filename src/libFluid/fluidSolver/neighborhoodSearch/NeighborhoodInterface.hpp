#pragma once

#include "FluidInclude.hpp"

#include <functional>


namespace LibFluid {


    /**
	 * @brief A slow but generic interface to allow other non templated components
	 * the access to the
     * neighborhood search.
	 * 
	 * In order to not oppose the NeighborhoodSearch as template argument to components

     * * like entities, that "rarely" access the neighborhood search, this interface was
	 * created. It wraps any
     * iterator into its own (not templated) iterator. Calls to
	 * the original iterator are realized through function
     * objects.
	 * The NeighborhoodInterface provides the basic requirements of the Neighbor object
	 * used in the
     * neighborhood searches.
	 */
    class NeighborhoodInterface {

      public:
        using particleIndex_t = pIndex_t;

        struct Neighbors;


        struct NeighborsIterator
        {

            Neighbors* data = nullptr;

            void* original_iterator = nullptr;

            bool operator==(const NeighborsIterator& other) const;

            bool operator!=(const NeighborsIterator& other) const;

            particleIndex_t& operator*();

            NeighborsIterator& operator++();

            const NeighborsIterator operator++(int);

            ~NeighborsIterator();

            NeighborsIterator(const NeighborsIterator& to_copy);

            NeighborsIterator() = default;
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


            NeighborsIterator begin();

            NeighborsIterator end();


            struct IteratorInterface
            {
                std::function<void*(void* it)> iterator_copy;
                std::function<void(void* it)> iterator_increment;
                std::function<bool(void* it1, void* it2)> iterator_equals;
                std::function<void(void* it)> iterator_delete;
                std::function<particleIndex_t*(void* it)> iterator_dereference;
                std::function<void*()> begin;
                std::function<void*()> end;
            } iterator_link;
        };


        Neighbors get_neighbors(particleIndex_t particleIndex);

        Neighbors get_neighbors(const vec2& position);
        Neighbors get_neighbors(const vec3& position);

        float get_search_radius();


        struct Interface
        {
            std::function<Neighbors(particleIndex_t particleIndex)> get_by_index;
            std::function<Neighbors(const vec2& position)> get_by_position;
            std::function<Neighbors(const vec3& position)> get_by_position_3d;
            std::function<float()> get_search_radius;
        } link;
    };


} // namespace FluidSolver