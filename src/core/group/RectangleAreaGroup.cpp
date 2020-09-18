#include "RectangleAreaGroup.hpp"

bool FluidSolver::RectangleAreaGroup::RectangleAreaGroupIterator::operator==(
        const FluidSolver::RectangleAreaGroup::RectangleAreaGroupIterator &other) const {
    return other.data == data && other.current == current;
}

bool FluidSolver::RectangleAreaGroup::RectangleAreaGroupIterator::operator!=(
        const FluidSolver::RectangleAreaGroup::RectangleAreaGroupIterator &other) const {
    return !(*this == other);
}

FluidSolver::RectangleAreaGroup::T &FluidSolver::RectangleAreaGroup::RectangleAreaGroupIterator::operator*() {
    return current;
}

FluidSolver::RectangleAreaGroup::RectangleAreaGroupIterator &
FluidSolver::RectangleAreaGroup::RectangleAreaGroupIterator::operator++() {
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

const FluidSolver::RectangleAreaGroup::RectangleAreaGroupIterator
FluidSolver::RectangleAreaGroup::RectangleAreaGroupIterator::operator++(int) {
    FLUID_ASSERT(data != nullptr)
    FLUID_ASSERT(data->collection != nullptr)
    RectangleAreaGroup::RectangleAreaGroupIterator copy = *this;
    ++(*this);
    return copy;
}

bool FluidSolver::RectangleAreaGroup::is_inside(pIndex_t index) const {
    FLUID_ASSERT(collection->is_type_present<MovementData>())
    FLUID_ASSERT(collection->size() > index)
    const vec2 &position = collection->get<MovementData>(index).position;

    if (position.x < area.left || position.x > area.right || position.y > area.top || position.y < area.bottom)
        return false;
    return true;
}

bool FluidSolver::RectangleAreaGroup::is_member(pIndex_t index) const {
    return inside == is_inside(index);
}

FluidSolver::RectangleAreaGroup::RectangleAreaGroupIterator FluidSolver::RectangleAreaGroup::begin() {
    RectangleAreaGroupIterator it;
    it.current = -1;
    it.data = this;
    ++it;
    return it;
}

FluidSolver::RectangleAreaGroup::RectangleAreaGroupIterator FluidSolver::RectangleAreaGroup::end() {
    FLUID_ASSERT(collection != nullptr)
    RectangleAreaGroupIterator it;
    it.data = this;
    it.current = collection->size();
    return it;
}
