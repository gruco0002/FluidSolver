#include "NeighborhoodInterface.hpp"



FluidSolver::NeighborhoodInterface::NeighborsIterator FluidSolver::NeighborhoodInterface::Neighbors::begin() {
	FLUID_ASSERT(iterator_link.begin != nullptr);
	NeighborsIterator it;
	it.data = this;
	it.original_iterator = iterator_link.begin();
	return it;
}

FluidSolver::NeighborhoodInterface::NeighborsIterator FluidSolver::NeighborhoodInterface::Neighbors::end() {
	FLUID_ASSERT(iterator_link.end != nullptr);
	NeighborsIterator it;
	it.data = this;
	it.original_iterator = iterator_link.end();
	return it;
}


bool FluidSolver::NeighborhoodInterface::NeighborsIterator::operator==(const NeighborsIterator& other) const {
	FLUID_ASSERT(data != nullptr);
	FLUID_ASSERT(data->iterator_link.iterator_equals != nullptr);

	return data->iterator_link.iterator_equals(this->original_iterator, other.original_iterator);
}

bool FluidSolver::NeighborhoodInterface::NeighborsIterator::operator!=(const NeighborsIterator& other) const {
	return !(*this == other);
}

FluidSolver::NeighborhoodInterface::particleIndex_t& FluidSolver::NeighborhoodInterface::NeighborsIterator::operator*() {
	FLUID_ASSERT(data != nullptr);
	FLUID_ASSERT(data->iterator_link.iterator_dereference != nullptr);
	FLUID_ASSERT(original_iterator != nullptr);

	return *data->iterator_link.iterator_dereference(original_iterator);
}

FluidSolver::NeighborhoodInterface::NeighborsIterator& FluidSolver::NeighborhoodInterface::NeighborsIterator::operator++() {
	FLUID_ASSERT(data != nullptr);
	FLUID_ASSERT(data->iterator_link.iterator_increment != nullptr);
	FLUID_ASSERT(original_iterator != nullptr);
	data->iterator_link.iterator_increment(this->original_iterator);
	return *this;
}

const FluidSolver::NeighborhoodInterface::NeighborsIterator FluidSolver::NeighborhoodInterface::NeighborsIterator::operator++(int) {	
	NeighborsIterator copy = *this;
	++(*this);
	return copy;
}

FluidSolver::NeighborhoodInterface::Neighbors FluidSolver::NeighborhoodInterface::get_neighbors(particleIndex_t particleIndex)
{
	FLUID_ASSERT(link.get_by_index != nullptr);
	return link.get_by_index(particleIndex);
}

FluidSolver::NeighborhoodInterface::Neighbors FluidSolver::NeighborhoodInterface::get_neighbors(const vec2& position)
{
	FLUID_ASSERT(link.get_by_position != nullptr);
	return link.get_by_position(position);
}

FluidSolver::NeighborhoodInterface::NeighborsIterator::~NeighborsIterator()
{
	FLUID_ASSERT(data != nullptr);
	FLUID_ASSERT(data->iterator_link.iterator_delete != nullptr);
	data->iterator_link.iterator_delete(original_iterator);
	original_iterator = nullptr;
	data = nullptr;
}

FluidSolver::NeighborhoodInterface::NeighborsIterator::NeighborsIterator(const NeighborsIterator& to_copy)
{
	FLUID_ASSERT(data != nullptr);
	FLUID_ASSERT(data->iterator_link.iterator_copy != nullptr);
	FLUID_ASSERT(original_iterator != nullptr);
	this->original_iterator = data->iterator_link.iterator_copy(to_copy.original_iterator);
	this->data = to_copy.data;
}

