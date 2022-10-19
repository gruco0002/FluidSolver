#pragma once

#include "LibFluidTypes.hpp"
#include "fluidSolver/ParticleCollection.hpp"
#include "group/Volume.hpp"

#include <memory>

namespace LibFluid {
class VolumeGroup {


      public:
        Volume volume = {};

        bool inside = false;

        std::shared_ptr<ParticleCollection> collection = nullptr;

        bool is_inside(size_t index) const;

        bool is_member(size_t index) const;

      public:
        struct VolumeGroupIterator;

        // iterator defines
        using T = size_t;
        using iterator = VolumeGroupIterator;
        using const_iterator = VolumeGroupIterator;
        using difference_type = ptrdiff_t;
        using size_type = size_t;
        using value_type = T;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;

        struct VolumeGroupIterator {
            const VolumeGroup* data;
            T current;

            bool operator==(const VolumeGroupIterator& other) const;

            bool operator!=(const VolumeGroupIterator& other) const;

            T& operator*();

            VolumeGroupIterator& operator++();

            const VolumeGroupIterator operator++(int);
        };

        VolumeGroupIterator begin();

        VolumeGroupIterator end();

        VolumeGroupIterator begin() const;

        VolumeGroupIterator end() const;

};

}