#pragma once

#include "FluidInclude.hpp"
#include "fluidSolver/ParticleCollection.hpp"

#include <unordered_set>

namespace LibFluid {
    class TagGroup {
      public:
        std::unordered_set<pTag_t> tags;

        ParticleCollection* collection = nullptr;

        bool is_member(pIndex_t index) const;

      public:
        struct TagGroupIterator;


        // iterator defines
        using T = pIndex_t;
        using iterator = TagGroupIterator;
        using const_iterator = TagGroupIterator;
        using difference_type = ptrdiff_t;
        using size_type = size_t;
        using value_type = T;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;

        struct TagGroupIterator {
            TagGroup* data;
            T current;

            bool operator==(const iterator& other) const;

            bool operator!=(const iterator& other) const;

            T& operator*();

            iterator& operator++();

            const iterator operator++(int);
        };

        iterator begin();

        iterator end();
    };
} // namespace FluidSolver
