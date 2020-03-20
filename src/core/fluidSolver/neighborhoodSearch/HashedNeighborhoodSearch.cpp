#include <core/FluidSolverException.hpp>
#include "HashedNeighborhoodSearch.hpp"

FluidSolver::HashedNeighborhoodSearch::GridKey
FluidSolver::HashedNeighborhoodSearch::GetGridCellByParticleID(particleIndex_t particleIndex) {
    glm::vec2 pos = particleCollection->GetPosition(particleIndex);
    return GetGridCellByPosition(pos);
}

void
FluidSolver::HashedNeighborhoodSearch::CreateGridEntryIfNecessary(FluidSolver::HashedNeighborhoodSearch::GridKey &key) {
    if (gridToParticles.find(key) == gridToParticles.end()) {
        // create entry
        gridToParticles[key] = std::list<particleIndex_t>();
    }
}

FluidSolver::HashedNeighborhoodSearch::GridKey
FluidSolver::HashedNeighborhoodSearch::GetGridCellByPosition(glm::vec2 &pos) {
    float tmpx = pos.x / gridCellSize;
    float tmpy = pos.y / gridCellSize;
    int32_t x = static_cast<int32_t>(tmpx);
    int32_t y = static_cast<int32_t>(tmpy);
    return {x, y};
}

void FluidSolver::HashedNeighborhoodSearch::UpdateGrid() {
    particleAmount_t particleCount = particleCollection->GetSize();
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

void FluidSolver::HashedNeighborhoodSearch::FindNeighbors() {
    // create buckets if not existing and / or clear it
    while (bucketsCreatedUntilIndex < particleCollection->GetSize()) {
        neighbors[bucketsCreatedUntilIndex] = std::pair<particleIndex_t, std::vector<particleAmount_t >>(0,
                                                                                                         std::vector<particleIndex_t>());
        currentStatus[bucketsCreatedUntilIndex] = {INT32_MAX, INT32_MAX};
        bucketsCreatedUntilIndex++;
    }


    // First update the grid
    UpdateGrid();

    for (particleIndex_t particleIndex = 0; particleIndex < particleCollection->GetSize(); particleIndex++) {
        // get position and grid cell
        glm::vec2 position = particleCollection->GetPosition(particleIndex);
        GridKey gridCell = GetGridCellByParticleID(particleIndex);

        // determine the grid cells needed to check
        std::vector<GridKey> toCheck;
        int32_t radiusInCellsFromTheCenter = ceil(radius / gridCellSize);

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
                glm::vec2 distVec = position - particleCollection->GetPosition(particle);
                if (glm::length(distVec) <= radius) {
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

std::shared_ptr<FluidSolver::Neighbors>
FluidSolver::HashedNeighborhoodSearch::GetNeighbors(FluidSolver::particleIndex_t particleIndex) {
    auto count = neighbors[particleIndex].first;
    if (count == 0) {
        return std::shared_ptr<Neighbors>(new NeighborsCompact(nullptr, 0));
    }
    return std::shared_ptr<Neighbors>(new NeighborsCompact(&(neighbors[particleIndex].second.front()), count));
}

std::shared_ptr<FluidSolver::Neighbors> FluidSolver::HashedNeighborhoodSearch::GetNeighbors(glm::vec2 position) {

    // get grid cell
    GridKey gridCell = GetGridCellByPosition(position);

    // determine the grid cells needed to check
    int32_t radiusInCellsFromTheCenter = ceil(radius / gridCellSize);
    std::vector<GridKey> toCheck;
    toCheck.reserve(((size_t) radiusInCellsFromTheCenter * 2 + 1) * ((size_t) radiusInCellsFromTheCenter * 2 + 1));


    for (int32_t x = gridCell.first - radiusInCellsFromTheCenter;
         x <= gridCell.first + radiusInCellsFromTheCenter; x++) {
        for (int32_t y = gridCell.second - radiusInCellsFromTheCenter;
             y <= gridCell.second + radiusInCellsFromTheCenter; y++) {
            toCheck.push_back({x, y});
        }
    }


    std::vector<particleIndex_t> out;
    out.reserve(32);
    // iterate over the grid cells and their particles to check for neighbors
    for (GridKey &key : toCheck) {
        if (gridToParticles.find(key) == gridToParticles.end())
            continue;

        for (uint32_t particle : gridToParticles[key]) {
            glm::vec2 distVec = position - particleCollection->GetPosition(particle);
            if (glm::length(distVec) <= radius) {
                // this is a neighbor, add it
                out.push_back(particle);
            }
        }
    }

    return std::shared_ptr<Neighbors>(new NeighborsCompactData(out));
}

FluidSolver::HashedNeighborhoodSearch::HashedNeighborhoodSearch(FluidSolver::IParticleCollection *particleCollection,
                                                                float radius) : INeighborhoodSearch(particleCollection,
                                                                                                    radius) {
    this->gridCellSize = radius;
}
