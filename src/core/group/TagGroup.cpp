#include "TagGroup.hpp"


bool FluidSolver::TagGroup::is_member(pIndex_t index) const {
    FLUID_ASSERT(collection != nullptr)
    FLUID_ASSERT(collection->is_type_present<ParticleInfo>())
    FLUID_ASSERT(collection->size() > index)
    pTag_t tag = collection->get<ParticleInfo>(index).tag;
    return tags.find(tag) != tags.end();
}

FluidSolver::TagGroup::iterator FluidSolver::TagGroup::begin() {
    iterator it;
    it.current = -1;
    it.data = this;
    ++it;
    return it;
}

FluidSolver::TagGroup::iterator FluidSolver::TagGroup::end() {
    FLUID_ASSERT(collection != nullptr)
    iterator it;
    it.data = this;
    it.current = collection->size();
    return it;
}

bool FluidSolver::TagGroup::TagGroupIterator::operator!=(const FluidSolver::TagGroup::iterator &other) const {
    return !(*this == other);
}

bool FluidSolver::TagGroup::TagGroupIterator::operator==(const FluidSolver::TagGroup::iterator &other) const {
    return other.data == data && other.current == current;
}

FluidSolver::TagGroup::T &FluidSolver::TagGroup::TagGroupIterator::operator*() {
    return current;
}

FluidSolver::TagGroup::iterator &FluidSolver::TagGroup::TagGroupIterator::operator++() {
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

const FluidSolver::TagGroup::iterator FluidSolver::TagGroup::TagGroupIterator::operator++(int) {
    FLUID_ASSERT(data != nullptr)
    FLUID_ASSERT(data->collection != nullptr)
    TagGroup::TagGroupIterator copy = *this;
    ++(*this);
    return copy;
}
