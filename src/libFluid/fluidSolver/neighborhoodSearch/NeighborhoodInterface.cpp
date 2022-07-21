#include "NeighborhoodInterface.hpp"


LibFluid::NeighborhoodInterface::NeighborsIterator LibFluid::NeighborhoodInterface::Neighbors::begin()
{
    FLUID_ASSERT(iterator_link.begin != nullptr);
    NeighborsIterator it;
    it.data = this;
    it.original_iterator = iterator_link.begin();
    return it;
}

LibFluid::NeighborhoodInterface::NeighborsIterator LibFluid::NeighborhoodInterface::Neighbors::end()
{
    FLUID_ASSERT(iterator_link.end != nullptr);
    NeighborsIterator it;
    it.data = this;
    it.original_iterator = iterator_link.end();
    return it;
}


bool LibFluid::NeighborhoodInterface::NeighborsIterator::operator==(const NeighborsIterator& other) const
{
    FLUID_ASSERT(data != nullptr);
    FLUID_ASSERT(data->iterator_link.iterator_equals != nullptr);

    return data->iterator_link.iterator_equals(this->original_iterator, other.original_iterator);
}

bool LibFluid::NeighborhoodInterface::NeighborsIterator::operator!=(const NeighborsIterator& other) const
{
    return !(*this == other);
}

LibFluid::NeighborhoodInterface::particleIndex_t& LibFluid::NeighborhoodInterface::NeighborsIterator::operator*()
{
    FLUID_ASSERT(data != nullptr);
    FLUID_ASSERT(data->iterator_link.iterator_dereference != nullptr);
    FLUID_ASSERT(original_iterator != nullptr);

    return *data->iterator_link.iterator_dereference(original_iterator);
}

LibFluid::NeighborhoodInterface::NeighborsIterator& LibFluid::NeighborhoodInterface::NeighborsIterator::
operator++()
{
    FLUID_ASSERT(data != nullptr);
    FLUID_ASSERT(data->iterator_link.iterator_increment != nullptr);
    FLUID_ASSERT(original_iterator != nullptr);
    data->iterator_link.iterator_increment(this->original_iterator);
    return *this;
}

const LibFluid::NeighborhoodInterface::NeighborsIterator LibFluid::NeighborhoodInterface::NeighborsIterator::
operator++(int)
{
    NeighborsIterator copy = *this;
    ++(*this);
    return copy;
}

LibFluid::NeighborhoodInterface::Neighbors LibFluid::NeighborhoodInterface::get_neighbors(
    particleIndex_t particleIndex)
{
    FLUID_ASSERT(link.get_by_index != nullptr);
    return link.get_by_index(particleIndex);
}

LibFluid::NeighborhoodInterface::Neighbors LibFluid::NeighborhoodInterface::get_neighbors(const vec2& position)
{
    FLUID_ASSERT(link.get_by_position != nullptr);
    return link.get_by_position(position);
}
LibFluid::NeighborhoodInterface::Neighbors LibFluid::NeighborhoodInterface::get_neighbors(const vec3& position)
{
    FLUID_ASSERT(link.get_by_position_3d != nullptr);
    return link.get_by_position_3d(position);
}

LibFluid::NeighborhoodInterface::NeighborsIterator::~NeighborsIterator()
{
    FLUID_ASSERT(data != nullptr);
    FLUID_ASSERT(data->iterator_link.iterator_delete != nullptr);
    data->iterator_link.iterator_delete(original_iterator);
    original_iterator = nullptr;
    data = nullptr;
}

LibFluid::NeighborhoodInterface::NeighborsIterator::NeighborsIterator(const NeighborsIterator& to_copy)
{
    FLUID_ASSERT(to_copy.data != nullptr);
    FLUID_ASSERT(to_copy.data->iterator_link.iterator_copy != nullptr);
    FLUID_ASSERT(to_copy.original_iterator != nullptr);
    this->original_iterator = to_copy.data->iterator_link.iterator_copy(to_copy.original_iterator);
    this->data = to_copy.data;
}

float LibFluid::NeighborhoodInterface::get_search_radius(){
    FLUID_ASSERT(this->link.get_search_radius != nullptr);
    return this->link.get_search_radius();
}
