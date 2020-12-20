#include "QuadraticNeighborhoodSearchDynamicAllocated.hpp"

#include <functional>

#include <core/parallelization/StdParallelForEach.hpp>

using parallel = FluidSolver::StdParallelForEach;

void FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::find_neighbors() {
	FLUID_ASSERT(collection != nullptr);
	FLUID_ASSERT(search_radius > 0.0f);

	// create map entry for particles if not already existing
	for (particleIndex_t i = 0; i < collection->size(); i++) {
		// check if already allocated
		if (neighbors.find(i) == neighbors.end())
			neighbors[i] = std::vector<particleIndex_t>();
	}

	// find the neighbors by quadratic search

	parallel::loop_for(0, collection->size(), [&](particleIndex_t particleIndex)
		{

			// delete old neighbors of particles in the map
			neighbors[particleIndex].clear();

			// calculate neighbors for this particle
			const glm::vec2& position = collection->get<MovementData>(particleIndex).position;
			for (particleIndex_t i = 0; i < collection->size(); i++) {
				glm::vec2 distVec = position - collection->get<MovementData>(i).position;
				if (glm::length(distVec) <= search_radius) {
					// this is a neighbor: add it to the list inside the map
					neighbors[particleIndex].push_back(i);
				}
			}

		});

}

FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::Neighbors
FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::get_neighbors(particleIndex_t particleIndex) {
	Neighbors n;
	n.position_based = false;
	n.data = this;
	n.of.particle = particleIndex;
	return n;
}

FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::Neighbors
FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::get_neighbors(const vec2& position) {
	Neighbors n;
	n.position_based = true;
	n.data = this;
	n.of.position = position;
	return n;
}

void FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::initialize() {

}

FluidSolver::NeighborhoodInterface FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::create_interface()
{
	auto res = NeighborhoodInterface();

	res.link.get_by_index = [this](particleIndex_t index) {
		auto neighbors = this->get_neighbors(index);

		auto n = NeighborhoodInterface::Neighbors();
		n.iterator_link.begin = [neighbors]() {
			auto real_it = neighbors.begin();
			return new NeighborsIterator(real_it);
		};
		n.iterator_link.end = [neighbors]() {
			auto real_it = neighbors.end();
			return new NeighborsIterator(real_it);
		};
		n.iterator_link.iterator_copy = [](void* it) {
			auto copy = new NeighborsIterator(*((NeighborsIterator*)it));
			return copy;
		};
		n.iterator_link.iterator_delete = [](void* it) {
			delete ((NeighborsIterator*)it);
		};
		n.iterator_link.iterator_dereference = [](void* it) {
			auto& index = *(*(NeighborsIterator*)it);
			return &index;
		};
		n.iterator_link.iterator_equals = [](void* it1, void* it2) {
			return *((NeighborsIterator*)it1) == *((NeighborsIterator*)it2);
		};
		n.iterator_link.iterator_increment = [](void* it) {
			++(*(NeighborsIterator*)it);
		};

		return n;
	};

	res.link.get_by_position = [this](const vec2& position) {

		auto neighbors = this->get_neighbors(position);

		auto n = NeighborhoodInterface::Neighbors();
		n.iterator_link.begin = [neighbors]() {
			auto real_it = neighbors.begin();
			return new NeighborsIterator(real_it);
		};
		n.iterator_link.end = [neighbors]() {
			auto real_it = neighbors.end();
			return new NeighborsIterator(real_it);
		};
		n.iterator_link.iterator_copy = [](void* it) {
			auto copy = new NeighborsIterator(*((NeighborsIterator*)it));
			return copy;
		};
		n.iterator_link.iterator_delete = [](void* it) {
			delete ((NeighborsIterator*)it);
		};
		n.iterator_link.iterator_dereference = [](void* it) {
			auto& index = *(*(NeighborsIterator*)it);
			return &index;
		};
		n.iterator_link.iterator_equals = [](void* it1, void* it2) {
			return *((NeighborsIterator*)it1) == *((NeighborsIterator*)it2);
		};
		n.iterator_link.iterator_increment = [](void* it) {
			++(*(NeighborsIterator*)it);
		};

		return n;
	};

	return res;
}


bool FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::NeighborsIterator::operator==(
	const FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::NeighborsIterator& other) const {
	return data->data == other.data->data && current == other.current;
}

bool FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::NeighborsIterator::operator!=(
	const FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::NeighborsIterator& other) const {
	return !(*this == other);
}

FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::particleIndex_t&
FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::NeighborsIterator::operator*() {
	FLUID_ASSERT(data != nullptr);
	FLUID_ASSERT(data->data != nullptr);
	if (!data->position_based) {
		FLUID_ASSERT(data->data->neighbors.find(data->of.particle) != data->data->neighbors.end());
		FLUID_ASSERT(data->data->neighbors[data->of.particle].size() > current);
		FLUID_ASSERT(current >= 0);
		return data->data->neighbors[data->of.particle][current];
	}
	else {
		FLUID_ASSERT(data->data->collection != nullptr);
		FLUID_ASSERT(data->data->collection->size() > current);
		return current;
	}
}

const FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::NeighborsIterator
FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::NeighborsIterator::operator++(int) {
	FLUID_ASSERT(data != nullptr);

	NeighborsIterator copy = *this;
	if (!data->position_based) {
		current++;
	}
	else {
		FLUID_ASSERT(data->data != nullptr);
		FLUID_ASSERT(data->data->collection != nullptr);
		FLUID_ASSERT(data->data->search_radius > 0.0f);
		auto collection = data->data->collection;
		current++;
		while (current < collection->size()) {
			const glm::vec2& position = collection->get<MovementData>(current).position;
			if (glm::length(data->of.position - position) <= data->data->search_radius) {
				break;
			}
			current++;
		}
	}
	return copy;
}

FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::NeighborsIterator&
FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::NeighborsIterator::operator++() {
	FLUID_ASSERT(data != nullptr);

	if (!data->position_based) {
		current++;
	}
	else {
		FLUID_ASSERT(data->data != nullptr);
		FLUID_ASSERT(data->data->collection != nullptr);
		FLUID_ASSERT(data->data->search_radius > 0.0f);
		auto collection = data->data->collection;
		current++;
		while (current < collection->size()) {
			const glm::vec2& position = collection->get<MovementData>(current).position;
			if (glm::length(data->of.position - position) <= data->data->search_radius) {
				break;
			}
			current++;
		}
	}
	return *this;
}

FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::NeighborsIterator
FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::Neighbors::begin() const {
	FLUID_ASSERT(data != nullptr);
	NeighborsIterator iterator;
	iterator.data = this;
	iterator.current = -1;
	iterator++;
	return iterator;
}

FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::NeighborsIterator
FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::Neighbors::end() const {
	FLUID_ASSERT(data != nullptr);
	NeighborsIterator iterator;
	iterator.data = this;
	if (!position_based) {
		FLUID_ASSERT(data->neighbors.find(of.particle) != data->neighbors.end());
		iterator.current = data->neighbors[of.particle].size();
	}
	else {
		FLUID_ASSERT(data->collection != nullptr);
		iterator.current = data->collection->size();
	}
	return iterator;
}


