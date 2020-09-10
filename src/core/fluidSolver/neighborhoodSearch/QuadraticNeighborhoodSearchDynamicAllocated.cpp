#include "QuadraticNeighborhoodSearchDynamicAllocated.hpp"
#include "../FluidAssert.hpp"

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
#pragma omp parallel for
    for (particleIndex_t particleIndex = 0; particleIndex < collection->size(); particleIndex++) {

        // delete old neighbors of particles in the map
        neighbors[particleIndex].clear();

        // calculate neighbors for this particle
        const glm::vec2 &position = collection->get<MovementData>(particleIndex).position;
        for (particleIndex_t i = 0; i < collection->size(); i++) {
            glm::vec2 distVec = position - collection->get<MovementData>(i).position;
            if (glm::length(distVec) <= search_radius) {
                // this is a neighbor: add it to the list inside the map
                neighbors[particleIndex].push_back(i);
            }
        }
    }

}

FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::Neighbors
FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::get_neighbors(size_t particleIndex) {
    Neighbors n;
    n.position_based = false;
    n.data = this;
    n.of.particle = particleIndex;
    return n;
}

FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::Neighbors
FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::get_neighbors(const glm::vec2 &position) {
    Neighbors n;
    n.position_based = true;
    n.data = this;
    n.of.position = position;
    return n;
}

bool FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::NeighborsIterator::operator==(
        const FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::NeighborsIterator &other) const {
    return data == other.data && current == other.current;
}

bool FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::NeighborsIterator::operator!=(
        const FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::NeighborsIterator &other) const {
    return !(*this == other);
}

FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::particleIndex_t &
FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::NeighborsIterator::operator*() {
    FLUID_ASSERT(data != nullptr);
    FLUID_ASSERT(data->data != nullptr);
    if (!data->position_based) {
        FLUID_ASSERT(data->data->neighbors.find(data->of.particle) != data->data->neighbors.end());
        FLUID_ASSERT(data->data->neighbors[data->of.particle].size() > current);
        FLUID_ASSERT(current >= 0);
        return data->data->neighbors[data->of.particle][current];
    } else {
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
    } else {
        FLUID_ASSERT(data->data != nullptr);
        FLUID_ASSERT(data->data->collection != nullptr);
        FLUID_ASSERT(data->data->search_radius > 0.0f);
        auto collection = data->data->collection;
        current++;
        while (current < collection->size()) {
            const glm::vec2 &position = collection->get<MovementData>(current).position;
            if (glm::length(data->of.position - position) <= data->data->search_radius) {
                break;
            }
            current++;
        }
    }
    return copy;
}

FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::NeighborsIterator &
FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::NeighborsIterator::operator++() {
    FLUID_ASSERT(data != nullptr);

    if (!data->position_based) {
        current++;
    } else {
        FLUID_ASSERT(data->data != nullptr);
        FLUID_ASSERT(data->data->collection != nullptr);
        FLUID_ASSERT(data->data->search_radius > 0.0f);
        auto collection = data->data->collection;
        current++;
        while (current < collection->size()) {
            const glm::vec2 &position = collection->get<MovementData>(current).position;
            if (glm::length(data->of.position - position) <= data->data->search_radius) {
                break;
            }
            current++;
        }
    }
    return *this;
}

FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::NeighborsIterator
FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::Neighbors::begin() {
    FLUID_ASSERT(data != nullptr);
    NeighborsIterator iterator;
    iterator.data = this;
    iterator.current = -1;
    iterator++;
    return iterator;
}

FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::NeighborsIterator
FluidSolver::QuadraticNeighborhoodSearchDynamicAllocated::Neighbors::end() {
    FLUID_ASSERT(data != nullptr);
    NeighborsIterator iterator;
    iterator.data = this;
    if (!position_based) {
        FLUID_ASSERT(data->neighbors.find(of.particle) != data->neighbors.end());
        iterator.current = data->neighbors[of.particle].size();
    } else {
        FLUID_ASSERT(data->collection != nullptr);
        iterator.current = data->collection->size();
    }
    return iterator;
}


