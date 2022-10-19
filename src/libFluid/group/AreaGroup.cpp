#include "AreaGroup.hpp"
namespace LibFluid {
    bool AreaGroup::AreaGroupIterator::operator==(
            const AreaGroup::AreaGroupIterator& other) const {
        return other.data == data && other.current == current;
    }

    bool AreaGroup::AreaGroupIterator::operator!=(
            const AreaGroup::AreaGroupIterator& other) const {
        return !(*this == other);
    }

    AreaGroup::T& AreaGroup::AreaGroupIterator::operator*() {
        return current;
    }

    AreaGroup::AreaGroupIterator& AreaGroup::
            AreaGroupIterator::operator++() {
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

    const AreaGroup::AreaGroupIterator AreaGroup::
            AreaGroupIterator::operator++(int) {
        FLUID_ASSERT(data != nullptr)
        FLUID_ASSERT(data->collection != nullptr)
        AreaGroup::AreaGroupIterator copy = *this;
        ++(*this);
        return copy;
    }

    bool AreaGroup::is_inside(size_t index) const {
        FLUID_ASSERT(collection->is_type_present<MovementData>())
        FLUID_ASSERT(collection->size() > index)
        const glm::vec2& position = collection->get<MovementData>(index).position;

        if (position.x < area.left || position.x > area.right || position.y > area.top || position.y < area.bottom)
            return false;
        return true;
    }

    bool AreaGroup::is_member(size_t index) const {
        return inside == is_inside(index);
    }

    AreaGroup::AreaGroupIterator AreaGroup::begin() {
        AreaGroupIterator it;
        it.current = -1;
        it.data = this;
        ++it;
        return it;
    }

    AreaGroup::AreaGroupIterator AreaGroup::end() {
        FLUID_ASSERT(collection != nullptr)
        AreaGroupIterator it;
        it.data = this;
        it.current = collection->size();
        return it;
    }

    AreaGroup::AreaGroupIterator AreaGroup::begin() const {
        AreaGroupIterator it;
        it.current = -1;
        it.data = this;
        ++it;
        return it;
    }

    AreaGroup::AreaGroupIterator AreaGroup::end() const {
        FLUID_ASSERT(collection != nullptr)
        AreaGroupIterator it;
        it.data = this;
        it.current = collection->size();
        return it;
    }
} // namespace FluidSolver