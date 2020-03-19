#include <core/FluidSolverException.hpp>
#include "HashedNeighborhoodSearch.hpp"

void FluidSolver::HashedNeighborhoodSearch::FindNeighbors(FluidSolver::IParticleCollection* particleCollection,
	float radius) {

	// First update the grid
	UpdateGrid(particleCollection);

	// call neighborhood search for each particle that is not a boundary particle
	INeighborhoodSearch::FindNeighbors(particleCollection, radius);
}

FluidSolver::Neighbors FluidSolver::HashedNeighborhoodSearch::GetParticleNeighbors(uint32_t particleIndex) {
	auto count = neighbors[particleIndex].first;
	if (count == 0) {
		return FluidSolver::Neighbors(nullptr, count);
	}
	return FluidSolver::Neighbors(&(neighbors[particleIndex].second.front()), count);
}

void FluidSolver::HashedNeighborhoodSearch::SetParticleCount(uint32_t particleCount) {
	// create buckets if not existing and / or clear it
	while (bucketsCreatedUntilIndex < particleCount) {
		neighbors[bucketsCreatedUntilIndex] = std::pair<uint32_t, std::vector<uint32_t >>(0, std::vector<uint32_t>());
		currentStatus[bucketsCreatedUntilIndex] = { INT32_MAX, INT32_MAX };
		bucketsCreatedUntilIndex++;
	}
}

void FluidSolver::HashedNeighborhoodSearch::FindNeighbors(uint32_t particleIndex,
	FluidSolver::IParticleCollection* particleCollection,
	float radius) {
	// get position and grid cell
	glm::vec2 position = particleCollection->GetPosition(particleIndex);
	GridKey gridCell = GetGridCellByParticleID(particleIndex, particleCollection);

	// determine the grid cells needed to check
	std::vector<GridKey> toCheck;
	int32_t radiusInCellsFromTheCenter = ceil(radius / gridCellSize);

	for (int32_t x = gridCell.first - radiusInCellsFromTheCenter;
		x <= gridCell.first + radiusInCellsFromTheCenter; x++) {
		for (int32_t y = gridCell.second - radiusInCellsFromTheCenter;
			y <= gridCell.second + radiusInCellsFromTheCenter; y++) {
			toCheck.push_back({ x, y });
		}
	}


	// reset data
	neighbors[particleIndex].first = 0;

	// iterate over the grid cells and their particles to check for neighbors
	for (GridKey& key : toCheck) {
		if (gridToParticles.find(key) == gridToParticles.end())
			continue;

		for (uint32_t particle : gridToParticles[key]) {
			glm::vec2 distVec = position - particleCollection->GetPosition(particle);
			if (glm::length(distVec) <= radius) {
				// this is a neighbor, add it
				if (neighbors[particleIndex].second.size() <= neighbors[particleIndex].first) {
					neighbors[particleIndex].second.push_back(particle);
				}
				else {
					neighbors[particleIndex].second[neighbors[particleIndex].first] = particle;
				}

				neighbors[particleIndex].first++;
			}
		}
	}
}

FluidSolver::HashedNeighborhoodSearch::GridKey
FluidSolver::HashedNeighborhoodSearch::GetGridCellByParticleID(uint32_t particleIndex,
	FluidSolver::IParticleCollection* particleCollection) {

	glm::vec2 pos = particleCollection->GetPosition(particleIndex);
	return GetGridCellByPosition(pos);
}

void
FluidSolver::HashedNeighborhoodSearch::CreateGridEntryIfNecessary(FluidSolver::HashedNeighborhoodSearch::GridKey& key) {
	if (gridToParticles.find(key) == gridToParticles.end()) {
		// create entry
		gridToParticles[key] = std::list<uint32_t>();
	}
}

FluidSolver::HashedNeighborhoodSearch::HashedNeighborhoodSearch(float gridCellSize) : gridCellSize(gridCellSize) {}

void
FluidSolver::HashedNeighborhoodSearch::GetParticleNeighborsByPosition(std::vector<uint32_t>& out, glm::vec2 position, float radius,
	IParticleCollection* particleCollection) {

	// get grid cell
	GridKey gridCell = GetGridCellByPosition(position);

	// determine the grid cells needed to check
	int32_t radiusInCellsFromTheCenter = ceil(radius / gridCellSize);
	std::vector<GridKey> toCheck;
	toCheck.reserve(((size_t)radiusInCellsFromTheCenter * 2 + 1) * ((size_t)radiusInCellsFromTheCenter * 2 + 1));
	

	for (int32_t x = gridCell.first - radiusInCellsFromTheCenter;
		x <= gridCell.first + radiusInCellsFromTheCenter; x++) {
		for (int32_t y = gridCell.second - radiusInCellsFromTheCenter;
			y <= gridCell.second + radiusInCellsFromTheCenter; y++) {
			toCheck.push_back({ x, y });
		}
	}


	// iterate over the grid cells and their particles to check for neighbors
	for (GridKey& key : toCheck) {
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

	
}

FluidSolver::HashedNeighborhoodSearch::GridKey
FluidSolver::HashedNeighborhoodSearch::GetGridCellByPosition(glm::vec2& pos) {
	float tmpx = pos.x / gridCellSize;
	float tmpy = pos.y / gridCellSize;
	int32_t x = static_cast<int32_t>(tmpx);
	int32_t y = static_cast<int32_t>(tmpy);
	return { x, y };
}

void FluidSolver::HashedNeighborhoodSearch::UpdateGrid(FluidSolver::IParticleCollection* particleCollection) {
	uint32_t particleCount = particleCollection->GetSize();
	// set every particle to the correct grid cell
	for (uint32_t i = 0; i < particleCount; i++) {
		// also neighbors have to be updated in the grid

		auto key = GetGridCellByParticleID(i, particleCollection);
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

void FluidSolver::HashedNeighborhoodSearch::SetupForPositionNeighborSearch(
	FluidSolver::IParticleCollection* particleCollection) {
	UpdateGrid(particleCollection);
}
