#include "AreaGroup.hpp"
namespace FluidSolver {
    bool AreaGroup::RectangleAreaGroupIterator::operator==(
            const AreaGroup::RectangleAreaGroupIterator& other) const {
        return other.data == data && other.current == current;
    }

    bool AreaGroup::RectangleAreaGroupIterator::operator!=(
            const AreaGroup::RectangleAreaGroupIterator& other) const {
        return !(*this == other);
    }

    AreaGroup::T& AreaGroup::RectangleAreaGroupIterator::operator*() {
        return current;
    }

    AreaGroup::RectangleAreaGroupIterator& AreaGroup::
            RectangleAreaGroupIterator::operator++() {
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

    const AreaGroup::RectangleAreaGroupIterator AreaGroup::
            RectangleAreaGroupIterator::operator++(int) {
        FLUID_ASSERT(data != nullptr)
        FLUID_ASSERT(data->collection != nullptr)
        AreaGroup::RectangleAreaGroupIterator copy = *this;
        ++(*this);
        return copy;
    }

    bool AreaGroup::is_inside(pIndex_t index) const {
        FLUID_ASSERT(collection->is_type_present<MovementData>())
        FLUID_ASSERT(collection->size() > index)
        const vec2& position = collection->get<MovementData>(index).position;

        if (position.x < area.left || position.x > area.right || position.y > area.top || position.y < area.bottom)
            return false;
        return true;
    }

    bool AreaGroup::is_member(pIndex_t index) const {
        return inside == is_inside(index);
    }

    AreaGroup::RectangleAreaGroupIterator AreaGroup::begin() {
        RectangleAreaGroupIterator it;
        it.current = -1;
        it.data = this;
        ++it;
        return it;
    }

    AreaGroup::RectangleAreaGroupIterator AreaGroup::end() {
        FLUID_ASSERT(collection != nullptr)
        RectangleAreaGroupIterator it;
        it.data = this;
        it.current = collection->size();
        return it;
    }

    AreaGroup::RectangleAreaGroupIterator AreaGroup::begin() const {
        RectangleAreaGroupIterator it;
        it.current = -1;
        it.data = this;
        ++it;
        return it;
    }

    AreaGroup::RectangleAreaGroupIterator AreaGroup::end() const {
        FLUID_ASSERT(collection != nullptr)
        RectangleAreaGroupIterator it;
        it.data = this;
        it.current = collection->size();
        return it;
    }
} // namespace FluidSolver