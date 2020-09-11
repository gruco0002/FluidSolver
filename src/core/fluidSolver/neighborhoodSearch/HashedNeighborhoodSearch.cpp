#include "HashedNeighborhoodSearch.hpp"
#include "../../FluidAssert.hpp"

namespace FluidSolver {

    void HashedNeighborhoodSearch::find_neighbors() {

        // First update the grid
        UpdateGrid();

#pragma omp parallel for
        for (particleIndex_t particleIndex = 0; particleIndex < collection->size(); particleIndex++) {
            auto type = collection->get<ParticleInfo>(particleIndex).type;
            if (type == ParticleType::ParticleTypeBoundary || type == ParticleType::ParticleTypeDead) {
                continue; // don't calculate unnecessary values for the boundary particles.
            }

            // get position and grid cell
            const glm::vec2 &position = collection->get<MovementData>(particleIndex).position;
            GridKey gridCell = GetGridCellByParticleID(particleIndex);

            // determine the grid cells needed to check
            std::vector<GridKey> toCheck;
            int32_t radiusInCellsFromTheCenter = ceil(search_radius / grid_cell_size);

            for (int32_t x = gridCell.first - radiusInCellsFromTheCenter;
                 x <= gridCell.first + radiusInCellsFromTheCenter; x++) {
                for (int32_t y = gridCell.second - radiusInCellsFromTheCenter;
                     y <= gridCell.second + radiusInCellsFromTheCenter; y++) {
                    toCheck.push_back({x, y});
                }
            }


            // reset data
            neighbors[particleIndex].first = 0;

            // iterate over the grid cells and their particles to check for neighbors
            for (GridKey &key : toCheck) {
                if (gridToParticles.find(key) == gridToParticles.end())
                    continue;

                for (particleIndex_t particle : gridToParticles[key]) {
                    glm::vec2 distVec = position - collection->get<MovementData>(particle).position;
                    if (glm::length(distVec) <= search_radius) {
                        // this is a neighbor, add it
                        if (neighbors[particleIndex].second.size() <= neighbors[particleIndex].first) {
                            neighbors[particleIndex].second.push_back(particle);
                        } else {
                            neighbors[particleIndex].second[neighbors[particleIndex].first] = particle;
                        }

                        neighbors[particleIndex].first++;
                    }
                }
            }
        }
    }

    HashedNeighborhoodSearch::Neighbors HashedNeighborhoodSearch::get_neighbors(size_t particleIndex) {
        Neighbors n;
        n.position_based = false;
        n.data = this;
        n.of.particle = particleIndex;
        return n;
    }

    HashedNeighborhoodSearch::Neighbors HashedNeighborhoodSearch::get_neighbors(const glm::vec2 &position) {
        Neighbors n;
        n.position_based = true;
        n.data = this;
        n.of.position = position;
        return n;
    }

    HashedNeighborhoodSearch::GridKey HashedNeighborhoodSearch::GetGridCellByParticleID(particleIndex_t particleIndex) {
        const glm::vec2 &pos = collection->get<MovementData>(particleIndex).position;
        return GetGridCellByPosition(pos);
    }

    void HashedNeighborhoodSearch::CreateGridEntryIfNecessary(const HashedNeighborhoodSearch::GridKey &key) {
        if (gridToParticles.find(key) == gridToParticles.end()) {
            // create entry
            gridToParticles[key] = std::list<particleIndex_t>();
        }
    }

    HashedNeighborhoodSearch::GridKey HashedNeighborhoodSearch::GetGridCellByPosition(const glm::vec2 &pos) {
        float tmpx = pos.x / grid_cell_size;
        float tmpy = pos.y / grid_cell_size;
        int32_t x = static_cast<int32_t>(tmpx);
        int32_t y = static_cast<int32_t>(tmpy);
        return {x, y};
    }

    void HashedNeighborhoodSearch::UpdateGrid() {
        // create buckets if not existing and / or clear it
        while (bucketsCreatedUntilIndex < collection->size()) {
            neighbors[bucketsCreatedUntilIndex] = std::pair<particleAmount_t, std::vector<particleIndex_t >>(0,
                                                                                                             std::vector<particleIndex_t>());
            currentStatus[bucketsCreatedUntilIndex] = {INT32_MAX, INT32_MAX};
            bucketsCreatedUntilIndex++;
        }


        particleAmount_t particleCount = collection->size();
        // set every particle to the correct grid cell
        for (particleIndex_t i = 0; i < particleCount; i++) {
            // also neighbors have to be updated in the grid

            auto key = GetGridCellByParticleID(i);
            if (currentStatus[i] == key)
                continue;

            // otherwise the gridcell of the particle changed

            // remove the particle from the old gridcell
            gridToParticles[currentStatus[i]].remove(i);

            // update current status
            currentStatus[i] = key;

            // add to new grid cell
            CreateGridEntryIfNecessary(key);
            gridToParticles[key].push_back(i);

        }
    }

    void HashedNeighborhoodSearch::initialize() {
        grid_cell_size = search_radius;
        neighbors.clear();
        currentStatus.clear();
        gridToParticles.clear();
    }

    bool HashedNeighborhoodSearch::NeighborsIterator::operator==(
            const HashedNeighborhoodSearch::NeighborsIterator &other) const {
        return data == other.data && current == other.current;
    }

    bool HashedNeighborhoodSearch::NeighborsIterator::operator!=(
            const HashedNeighborhoodSearch::NeighborsIterator &other) const {
        return !(*this == other);
    }

    HashedNeighborhoodSearch::particleIndex_t &HashedNeighborhoodSearch::NeighborsIterator::operator*() {
        FLUID_ASSERT(data != nullptr)
        FLUID_ASSERT(current >= 0)
        FLUID_ASSERT(data->data != nullptr)
        if (!data->position_based) {
            FLUID_ASSERT(data->data->neighbors.find(data->of.particle) != data->data->neighbors.end())
            FLUID_ASSERT(data->data->neighbors[data->of.particle].first > current);
            FLUID_ASSERT(data->data->neighbors[data->of.particle].second.size() > current);
            return data->data->neighbors[data->of.particle].second[current];
        } else {
            FLUID_ASSERT(data->data->collection != nullptr);
            FLUID_ASSERT(data->data->collection->size() > current);
            return current;
        }
    }

    HashedNeighborhoodSearch::NeighborsIterator &HashedNeighborhoodSearch::NeighborsIterator::operator++() {
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

    const HashedNeighborhoodSearch::NeighborsIterator HashedNeighborhoodSearch::NeighborsIterator::operator++(int) {
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

    HashedNeighborhoodSearch::NeighborsIterator HashedNeighborhoodSearch::Neighbors::begin() {
        FLUID_ASSERT(data != nullptr);
        NeighborsIterator iterator;
        iterator.data = this;
        iterator.current = -1;
        iterator++;
        return iterator;
    }

    HashedNeighborhoodSearch::NeighborsIterator HashedNeighborhoodSearch::Neighbors::end() {
        FLUID_ASSERT(data != nullptr);
        NeighborsIterator iterator;
        iterator.data = this;
        if (!position_based) {
            FLUID_ASSERT(data->neighbors.find(of.particle) != data->neighbors.end());
            FLUID_ASSERT(data->neighbors[of.particle].first <= data->neighbors[of.particle].second.size())
            iterator.current = data->neighbors[of.particle].first;
        } else {
            FLUID_ASSERT(data->collection != nullptr);
            iterator.current = data->collection->size();
        }
        return iterator;
    }
}
