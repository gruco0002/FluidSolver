#include "VolumeGroup.hpp"

#include "fluidSolver/ParticleCollection.hpp"

namespace LibFluid
{

    bool VolumeGroup::VolumeGroupIterator::operator==(const VolumeGroup::VolumeGroupIterator &other) const
    {
        return other.data == data && other.current == current;
    }

    bool VolumeGroup::VolumeGroupIterator::operator!=(const VolumeGroup::VolumeGroupIterator &other) const
    {
        return !(*this == other);
    }

    VolumeGroup::T &VolumeGroup::VolumeGroupIterator::operator*()
    {
        return current;
    }

    VolumeGroup::VolumeGroupIterator &VolumeGroup::VolumeGroupIterator::operator++()
    {
        FLUID_ASSERT(data != nullptr)
        FLUID_ASSERT(data->collection != nullptr)
        current++;
        while (current < data->collection->size())
        {
            if (data->is_member(current))
                break;
            current++;
        }
        return *this;
    }

    const VolumeGroup::VolumeGroupIterator VolumeGroup::VolumeGroupIterator::operator++(int)
    {
        FLUID_ASSERT(data != nullptr)
        FLUID_ASSERT(data->collection != nullptr)
        VolumeGroup::VolumeGroupIterator copy = *this;
        ++(*this);
        return copy;
    }

    bool VolumeGroup::is_inside(size_t index) const
    {
        FLUID_ASSERT(collection->is_type_present<MovementData3D>())
        FLUID_ASSERT(collection->size() > index)
        const auto &position = collection->get<MovementData3D>(index).position;

        auto max_corner = volume.center + volume.distance_from_center;
        auto min_corner = volume.center - volume.distance_from_center;

        if (position.x < min_corner.x || position.x > max_corner.x || position.y > max_corner.y ||
            position.y < min_corner.y || position.z < min_corner.z || position.z > max_corner.z)
            return false;
        return true;
    }

    bool VolumeGroup::is_member(size_t index) const
    {
        return inside == is_inside(index);
    }

    VolumeGroup::VolumeGroupIterator VolumeGroup::begin()
    {
        VolumeGroupIterator it;
        it.current = -1;
        it.data = this;
        ++it;
        return it;
    }

    VolumeGroup::VolumeGroupIterator VolumeGroup::end()
    {
        FLUID_ASSERT(collection != nullptr)
        VolumeGroupIterator it;
        it.data = this;
        it.current = collection->size();
        return it;
    }

    VolumeGroup::VolumeGroupIterator VolumeGroup::begin() const
    {
        VolumeGroupIterator it;
        it.current = -1;
        it.data = this;
        ++it;
        return it;
    }

    VolumeGroup::VolumeGroupIterator VolumeGroup::end() const
    {
        FLUID_ASSERT(collection != nullptr)
        VolumeGroupIterator it;
        it.data = this;
        it.current = collection->size();
        return it;
    }
} // namespace LibFluid
