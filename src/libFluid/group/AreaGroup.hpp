#pragma once

#include "fluidSolver/ParticleCollection.hpp"

#include <memory>

namespace LibFluid {

    class AreaGroup {
      public:
        Area area = {};

        bool inside = false;

        std::shared_ptr<ParticleCollection> collection = nullptr;

        bool is_inside(pIndex_t index) const;

        bool is_member(pIndex_t index) const;

      public:
        struct AreaGroupIterator;

        // iterator defines
        using T = pIndex_t;
        using iterator = AreaGroupIterator;
        using const_iterator = AreaGroupIterator;
        using difference_type = ptrdiff_t;
        using size_type = size_t;
        using value_type = T;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;

        struct AreaGroupIterator {
            const AreaGroup* data;
            T current;

            bool operator==(const AreaGroupIterator& other) const;

            bool operator!=(const AreaGroupIterator& other) const;

            T& operator*();

            AreaGroupIterator& operator++();

            const AreaGroupIterator operator++(int);
        };

        AreaGroupIterator begin();

        AreaGroupIterator end();

        AreaGroupIterator begin() const;

        AreaGroupIterator end() const;
    };


} // namespace FluidSolver
