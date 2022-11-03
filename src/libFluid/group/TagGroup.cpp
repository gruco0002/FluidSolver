#include "TagGroup.hpp"

namespace LibFluid {

    bool TagGroup::is_member(size_t index) const {
        FLUID_ASSERT(collection != nullptr)
        FLUID_ASSERT(collection->is_type_present<ParticleInfo>())
        FLUID_ASSERT(collection->size() > index)
        uint32_t tag = collection->get<ParticleInfo>(index).tag;
        return tags.find(tag) != tags.end();
    }

    TagGroup::iterator TagGroup::begin() {
        iterator it;
        it.current = -1;
        it.data = this;
        ++it;
        return it;
    }

    TagGroup::iterator TagGroup::end() {
        FLUID_ASSERT(collection != nullptr)
        iterator it;
        it.data = this;
        it.current = collection->size();
        return it;
    }

    bool TagGroup::TagGroupIterator::operator!=(const TagGroup::iterator& other) const {
        return !(*this == other);
    }

    bool TagGroup::TagGroupIterator::operator==(const TagGroup::iterator& other) const {
        return other.data == data && other.current == current;
    }

    TagGroup::T& TagGroup::TagGroupIterator::operator*() {
        return current;
    }

    TagGroup::iterator& TagGroup::TagGroupIterator::operator++() {
        FLUID_ASSERT(data != nullptr)
        FLUID_ASSERT(data->collection != nullptr)
        current++;
        while (current < data->collection->size()) {
            if (data->is_member(current))
                break;
            current++;
        }
        return *this;
    }

    const TagGroup::iterator TagGroup::TagGroupIterator::operator++(int) {
        FLUID_ASSERT(data != nullptr)
        FLUID_ASSERT(data->collection != nullptr)
        TagGroup::TagGroupIterator copy = *this;
        ++(*this);
        return copy;
    }
} // namespace FluidSolver