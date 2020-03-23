#include "CompactHashingNeighborhoodSearch.hpp"

FluidSolver::CompactHashingNeighborhoodSearch::CompactHashingNeighborhoodSearch(
        FluidSolver::IParticleCollection *particleCollection, float radius) : INeighborhoodSearch(particleCollection,
                                                                                                  radius), storage(40,
                                                                                                                   particleCollection->GetSize()) {
    cellSize = radius;
    particleCollectionIndicesChangedCounter = particleCollection->GetIndicesChangedCounter();
    initStructure = true;
}

void FluidSolver::CompactHashingNeighborhoodSearch::FindNeighbors() {
    // update the underlying data structure
    UpdateDataStructure();

    // check storage
    if (storage.getTotalParticleCount() < particleCollection->GetSize()) {
        storage.setTotalParticleCount(particleCollection->GetSize());
    }

    // searching neighbors
    for (particleIndex_t particleIndex = 0; particleIndex < particleCollection->GetSize(); particleIndex++) {

        particleAmount_t neighborCount = 0;
        particleIndex_t *data = storage.GetDataPtr(particleIndex);

        auto pos = particleCollection->GetPosition(particleIndex);

        auto cell = CalculateCellCoordinates(particleIndex);
        FindNeighborsForCellForParticle(pos, cell, neighborCount, data);

        // left
        auto alternatedCell = GridCell(cell.x - 1, cell.y);
        FindNeighborsForCellForParticle(pos, alternatedCell, neighborCount, data);
        // right
        alternatedCell = GridCell(cell.x + 1, cell.y);
        FindNeighborsForCellForParticle(pos, alternatedCell, neighborCount, data);
        // top
        alternatedCell = GridCell(cell.x, cell.y + 1);
        FindNeighborsForCellForParticle(pos, alternatedCell, neighborCount, data);
        // bottom
        alternatedCell = GridCell(cell.x, cell.y - 1);
        FindNeighborsForCellForParticle(pos, alternatedCell, neighborCount, data);
        // left bottom
        alternatedCell = GridCell(cell.x - 1, cell.y - 1);
        FindNeighborsForCellForParticle(pos, alternatedCell, neighborCount, data);
        // left top
        alternatedCell = GridCell(cell.x - 1, cell.y + 1);
        FindNeighborsForCellForParticle(pos, alternatedCell, neighborCount, data);
        // right bottom
        alternatedCell = GridCell(cell.x + 1, cell.y - 1);
        FindNeighborsForCellForParticle(pos, alternatedCell, neighborCount, data);
        // right top
        alternatedCell = GridCell(cell.x + 1, cell.y + 1);
        FindNeighborsForCellForParticle(pos, alternatedCell, neighborCount, data);

        storage.SetCount(particleIndex, neighborCount);

    }


}

size_t FluidSolver::CompactHashingNeighborhoodSearch::GetOrAquireStorageCell(size_t hash, const GridCell &cell) {


    if (hashTable[hash].info.attributes.hasAValue == 0) {
        // the hash value is not used at the moment, set it to used and note the cell
        hashTable[hash].info.attributes.hasAValue = 1;
        hashTable[hash].gridCellUsingThis = cell;

        auto storageSection = AquireNewCellStorageSection();
        hashTable[hash].storageSectionMappedTo = storageSection;
        // write grid cell coordinates to storage section header
        cellStorage[storageSection * storageSectionSize].particleGridCell = cell;
        return storageSection;

    } else {
        // the hash value is already in use, now testing which cell uses this
        if (hashTable[hash].gridCellUsingThis == cell) {
            // no problem found
            return hashTable[hash].storageSectionMappedTo;
        } else {
            // this particle is in another cell than the hashTable hence a hash collision occured
            hashTable[hash].info.attributes.hashCollisionHappened = 1;
            // resolve the hash collision
            size_t newHash = (hash + 1) % hashTableSize;
            while (hashTable[newHash].gridCellUsingThis != cell && hashTable[newHash].info.attributes.hasAValue == 1) {
                newHash = (newHash + 1) % hashTableSize;
                if (newHash == hash) {
                    throw std::logic_error("No free hash map entry was found!");
                }
            }
            // we resolved the hash collision by finding a cell that is either free to use or already reserved for this cell
            return GetOrAquireStorageCell(newHash, cell);
        }
    }
}

FluidSolver::NeighborsCompact
FluidSolver::CompactHashingNeighborhoodSearch::GetNeighbors(FluidSolver::particleIndex_t particleIndex) {
    return NeighborsCompact(storage.GetDataPtr(particleIndex), storage.GetCount(particleIndex));
}

FluidSolver::NeighborsCompactData FluidSolver::CompactHashingNeighborhoodSearch::GetNeighbors(glm::vec2 position) {
    throw std::logic_error("Not implemented");
    auto t = std::vector<particleIndex_t>();
    return NeighborsCompactData(t);
}

size_t FluidSolver::CompactHashingNeighborhoodSearch::CalculateHashValue(const GridCell &gridCell) {
    return ((uint64_t) (gridCell.x * primeOne) ^ (uint64_t) (gridCell.y * primeTwo)) % hashTableSize;
}

FluidSolver::CompactHashingNeighborhoodSearch::GridCell
FluidSolver::CompactHashingNeighborhoodSearch::CalculateCellCoordinates(FluidSolver::particleIndex_t particleIndex) {
    auto pos = particleCollection->GetPosition(particleIndex);
    GridCell cell;
    cell.x = (int64_t) floor(pos.x / cellSize); // a simple cast to int would round towards zero, hence cause errors
    cell.y = (int64_t) floor(pos.y / cellSize);
    return cell;
}

void FluidSolver::CompactHashingNeighborhoodSearch::AddParticleToStorageSection(size_t storageSection,
                                                                                FluidSolver::particleIndex_t particleIndex) {

    auto realIndexOfSection = storageSection * storageSectionSize;
    // the first field of the storage section states how many particle indices are stored within this section

    cellStorage[realIndexOfSection].particleIndex = cellStorage[realIndexOfSection].particleIndex + 1;
    auto count = cellStorage[realIndexOfSection].particleIndex;

    if (count >= storageSectionSize)
        throw std::logic_error("Tried to add a particle to a cell storage, but the cell storage was already full.");

    auto storageIndex = realIndexOfSection + count; // this is correct since the first index is
    cellStorage[storageIndex].particleIndex = particleIndex;
}

size_t FluidSolver::CompactHashingNeighborhoodSearch::AquireNewCellStorageSection() {
    // search for empty cells
    for (size_t i = 0; i < cellStorage.size() / storageSectionSize; i++) {
        auto index = i * storageSectionSize;
        if (cellStorage[index].particleIndex == 0)
            return i; // cell storage section i is empty
    }

    // there was no empty storage section: resize the array to fit more section
    auto index = cellStorage.size() / storageSectionSize;
    cellStorage.resize(cellStorage.size() + storageSectionSize, GridCellParticleHandle());
    return index;
}

void FluidSolver::CompactHashingNeighborhoodSearch::TotalResetSearch() {
    // reset and resize the hash table as needed
    hashTableSize = 2 * particleCollection->GetSize();
    if (this->hashTable.size() != hashTableSize) {
        hashTable.resize(hashTableSize);
    }

    // reset data
    // reset hash table
    for (size_t i = 0; i < hashTable.size(); i++) {
        // resetting the hash table
        hashTable[i].info.value = 0;
    }
    // reset the cell storage
    for (size_t i = 0; i < cellStorage.size() / storageSectionSize; i++) {
        auto index = i * storageSectionSize;
        cellStorage[index].particleIndex = 0; // reset the cell storage counts, but this is only valid of the cellStorageSize did not change
    }

    // insert all particles
    for (particleIndex_t particleIndex = 0; particleIndex < particleCollection->GetSize(); particleIndex++) {
        if (particleCollection->GetParticleType(particleIndex) == ParticleType::ParticleTypeDead)
            continue;
        auto cell = CalculateCellCoordinates(particleIndex);
        auto hash = CalculateHashValue(cell);
        auto storageSection = GetOrAquireStorageCell(hash, cell);
        AddParticleToStorageSection(storageSection, particleIndex);

    }
}

void FluidSolver::CompactHashingNeighborhoodSearch::SearchByDifference() {
    // this function exploits temporal coherence

    // 1. calculate new cell coordinates of each particle
    // 1.1 if the cell coordinate has changed, add particle to a moving list (or mark the particle)
    //     this is realised by then comparing the newly calculated grid cell coordinates against the storage header grid coordinates
    size_t toMoveCount = 0;
    for (size_t i = 0; i < cellStorage.size(); i++) {
        if (i % storageSectionSize == 0)
            continue; // this entry would be a cell header -> do not process it
        auto headerIndex = i - (i % storageSectionSize);

        cellStorage[i].particleGridCell = CalculateCellCoordinates(cellStorage[i].particleIndex);
        if (cellStorage[headerIndex].particleGridCell != cellStorage[i].particleGridCell)
            toMoveCount++;
    }

    // 2. remove the marked particles from the cell storage, add the others to the moving list
    std::vector<GridCellParticleHandle> toMove(toMoveCount);
    toMoveCount = 0;
    for (size_t i = 0; i < cellStorage.size() / storageSectionSize; i++) {
        auto headerIndex = i * storageSectionSize;
        auto storageCellCount = cellStorage[headerIndex].particleIndex;

        for (size_t c = storageCellCount ; c >= 1; c--) {
            auto particleHandleIndex = headerIndex  + c;
            if (cellStorage[particleHandleIndex].particleGridCell == cellStorage[headerIndex].particleGridCell)
                continue; // the particle handle is in the correct cell storage

            // the particle is in the wrong cell, remove it from the storage section, add it to the toMove vector and ensure that the storage stays compact
            toMove[toMoveCount] = cellStorage[particleHandleIndex];
            toMoveCount++;
            storageCellCount--;
            cellStorage[headerIndex].particleIndex = storageCellCount;

            if (c - 1 < storageCellCount) {
                // there are handles after the one that we deleted, move the handles after this one up such that the storage cell stays compact
                for (size_t reorder = c - 1; reorder < storageCellCount; reorder++) {
                    auto reorderHandleIndex = headerIndex + 1 + reorder;
                    cellStorage[reorder] = cellStorage[reorder + 1];
                }
            }

        }

    }

    // 3. insert the particles back into the storage at the correct location
    for (size_t i = 0; i < toMove.size(); i++) {
        // insert by handle
        auto hash = CalculateHashValue(toMove[i].particleGridCell);
        auto storageSection = GetOrAquireStorageCell(hash, toMove[i].particleGridCell);
        AddParticleToStorageSection(storageSection, toMove[i].particleIndex);
    }
}

void FluidSolver::CompactHashingNeighborhoodSearch::UpdateDataStructure() {
    if (initStructure || particleCollectionIndicesChangedCounter != particleCollection->GetIndicesChangedCounter()) {
        TotalResetSearch(); // use this on init and if particle collection size changed or indices of particles in the collection changed (due to sorting).
        initStructure = false;
        particleCollectionIndicesChangedCounter = particleCollection->GetIndicesChangedCounter();
    } else {
        SearchByDifference(); // else use this one
    }
}

void FluidSolver::CompactHashingNeighborhoodSearch::FindNeighborsForCellForParticle(
        const glm::vec2 &particlePosition,
        const FluidSolver::CompactHashingNeighborhoodSearch::GridCell &gridCell,
        FluidSolver::particleAmount_t &neighborCount, FluidSolver::particleIndex_t *data) {

    // checking if a table entry exists
    auto hash = CalculateHashValue(gridCell);
    if (hashTable[hash].info.attributes.hasAValue == 0)
        return; // grid cell is empty

    // checking for correct hash table entry
    auto newHash = hash;
    while ((hashTable[newHash].gridCellUsingThis != gridCell || hashTable[newHash].info.attributes.hasAValue == 0) &&
           hashTable[hash].info.attributes.hashCollisionHappened == 1) {
        newHash = (newHash + 1) % hashTableSize;
        if (newHash == hash)
            return; // there is no hash table entry corresponding to the grid cell
    }
    hash = newHash;

    auto handle = hashTable[hash];
    for (size_t i = 0; i < cellStorage[handle.storageSectionMappedTo * storageSectionSize].particleIndex; i++) {
        auto index = handle.storageSectionMappedTo * storageSectionSize + 1 + i;
        auto neighborIndex = cellStorage[index].particleIndex;

        // check for neighborhood
        auto neighborPos = particleCollection->GetPosition(neighborIndex);
        if (glm::length(particlePosition - neighborPos) <= radius) {
            // we found a neighbor
            if (neighborCount >= storage.GetNeighborStorageSizePerParticle())
                throw std::logic_error(
                        "Trying to add a new neighbor to the storage but the memory space allocated would be exceeded!");
            *(data + neighborCount) = neighborIndex;
            neighborCount++;
        }

    }


}


bool FluidSolver::CompactHashingNeighborhoodSearch::GridCell::operator==(
        const FluidSolver::CompactHashingNeighborhoodSearch::GridCell &rhs) const {
    return x == rhs.x &&
           y == rhs.y;
}

bool FluidSolver::CompactHashingNeighborhoodSearch::GridCell::operator!=(
        const FluidSolver::CompactHashingNeighborhoodSearch::GridCell &rhs) const {
    return !(rhs == *this);
}

FluidSolver::CompactHashingNeighborhoodSearch::GridCell::GridCell(int32_t x, int32_t y) {
    this->x = x;
    this->y = y;
}

FluidSolver::CompactHashingNeighborhoodSearch::NeighborStorage::NeighborStorage(
        FluidSolver::particleAmount_t neighborStorageSizePerParticle, FluidSolver::particleAmount_t totalParticleCount)
        : neighborStorageSizePerParticle(neighborStorageSizePerParticle), totalParticleCount(totalParticleCount) {
    ResizeVector();
}

void FluidSolver::CompactHashingNeighborhoodSearch::NeighborStorage::ResizeVector() {
    neighbors.resize((neighborStorageSizePerParticle + 1) * totalParticleCount);
}

void
FluidSolver::CompactHashingNeighborhoodSearch::NeighborStorage::SetCount(FluidSolver::particleIndex_t particleIndex,
                                                                         FluidSolver::particleAmount_t count) {
    neighbors[particleIndex * (neighborStorageSizePerParticle + 1)] = count;
}

FluidSolver::particleIndex_t *
FluidSolver::CompactHashingNeighborhoodSearch::NeighborStorage::GetDataPtr(FluidSolver::particleIndex_t particleIndex) {
    return &neighbors[particleIndex * (neighborStorageSizePerParticle + 1) + 1];
}

FluidSolver::particleAmount_t
FluidSolver::CompactHashingNeighborhoodSearch::NeighborStorage::GetNeighborStorageSizePerParticle() {
    return neighborStorageSizePerParticle;
}

void FluidSolver::CompactHashingNeighborhoodSearch::NeighborStorage::SetNeighborStorageSizePerParticle(
        FluidSolver::particleAmount_t size) {
    this->neighborStorageSizePerParticle = size;
    ResizeVector();
}

FluidSolver::particleAmount_t
FluidSolver::CompactHashingNeighborhoodSearch::NeighborStorage::getTotalParticleCount() const {
    return totalParticleCount;
}

void FluidSolver::CompactHashingNeighborhoodSearch::NeighborStorage::setTotalParticleCount(
        FluidSolver::particleAmount_t totalParticleCount) {
    NeighborStorage::totalParticleCount = totalParticleCount;
    ResizeVector();
}

FluidSolver::particleAmount_t
FluidSolver::CompactHashingNeighborhoodSearch::NeighborStorage::GetCount(FluidSolver::particleIndex_t particleIndex) {
    return neighbors[particleIndex * (neighborStorageSizePerParticle + 1)];
}
