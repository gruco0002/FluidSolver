#include "CompactHashingNeighborhoodSearch.hpp"

FluidSolver::CompactHashingNeighborhoodSearch::CompactHashingNeighborhoodSearch(
        FluidSolver::IParticleCollection *particleCollection, float radius) :
        INeighborhoodSearch(particleCollection, radius),
        storage(150, particleCollection->GetSize()),
        cellStorage(CellStorage(10)),
        hashTable(HashTable(particleCollection->GetSize() * 2)) {
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
        if (particleCollection->GetParticleType(particleIndex) == ParticleType::ParticleTypeDead) {
            continue;
        }

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

FluidSolver::NeighborsCompact
FluidSolver::CompactHashingNeighborhoodSearch::GetNeighbors(FluidSolver::particleIndex_t particleIndex) {
    return NeighborsCompact(storage.GetDataPtr(particleIndex), storage.GetCount(particleIndex));
}

FluidSolver::NeighborsCompactData FluidSolver::CompactHashingNeighborhoodSearch::GetNeighbors(glm::vec2 position) {
    throw std::logic_error("Not implemented");
    auto t = std::vector<particleIndex_t>();
    return NeighborsCompactData(t);
}


FluidSolver::CompactHashingNeighborhoodSearch::GridCell
FluidSolver::CompactHashingNeighborhoodSearch::CalculateCellCoordinates(FluidSolver::particleIndex_t particleIndex) {
    auto pos = particleCollection->GetPosition(particleIndex);
    GridCell cell;
    cell.x = (int64_t) floor(pos.x / cellSize); // a simple cast to int would round towards zero, hence cause errors
    cell.y = (int64_t) floor(pos.y / cellSize);
    return cell;
}


void FluidSolver::CompactHashingNeighborhoodSearch::RegenerateAllDataStructures() {
    // reset data
    hashTable = CompactHashingNeighborhoodSearch::HashTable(particleCollection->GetSize() * 2);
    cellStorage.ClearStorage();

    // insert all particles
    for (particleIndex_t particleIndex = 0; particleIndex < particleCollection->GetSize(); particleIndex++) {
        auto cell = CalculateCellCoordinates(particleIndex);
        size_t storageSection;
        if (!hashTable.GetValueByKey(cell, storageSection)) {
            storageSection = cellStorage.GetEmptyStorageSection();
            hashTable.SetValueByKey(cell, storageSection);
        }
        cellStorage.AddParticleToStorageSection(storageSection, particleIndex, cell);
    }
}

void FluidSolver::CompactHashingNeighborhoodSearch::SearchByDifference() {
    // this function exploits temporal coherence

    // 1. calculate new cell coordinates of each particle
    // 1.1 if the cell coordinate has changed, add particle to a remove and add list
    //     this is realised by then comparing the newly calculated grid cell coordinates against the storage header grid coordinates

    std::vector<std::pair<size_t, CellStorage::GridCellParticleHandle>> removeList;

    auto begin = hashTable.begin();
    auto end = hashTable.end();
    for (auto current = begin; current != end; current++) {
        auto cell = (*current);
        size_t storageSection;
        if (!hashTable.GetValueByKey(cell, storageSection)) {
            continue;
        }

        auto sectionBegin = cellStorage.GetStorageSectionDataBegin(storageSection);
        auto sectionEnd = cellStorage.GetStorageSectionDataEnd(storageSection);
        auto sectionGridCell = cellStorage.GetStorageSectionGridCell(storageSection);

        for (auto currentSection = sectionBegin; currentSection != sectionEnd; currentSection++) {
            auto &handle = *currentSection;
            handle.particleGridCell = CalculateCellCoordinates(handle.particleIndex.value);
            if (handle.particleGridCell != sectionGridCell) {
                removeList.push_back({storageSection, CellStorage::GridCellParticleHandle(handle)});
            }
        }
    }


    // 2. remove the particles from the list and add it again
    for (auto ele: removeList) {
        cellStorage.RemoveParticleFromStorageSection(ele.first, ele.second.particleIndex.value);

        if (cellStorage.GetStorageSectionElementCount(ele.first)) {
            // the cell storage became empty and hence the mapping has to be removed since empty storage sections do not belong to anyone
            auto oldGridKey = cellStorage.GetStorageSectionGridCell(ele.first);
            hashTable.RemoveKey(oldGridKey);
        }

        size_t storageSection;
        if (!hashTable.GetValueByKey(ele.second.particleGridCell, storageSection)) {
            storageSection = cellStorage.GetEmptyStorageSection();
            hashTable.SetValueByKey(ele.second.particleGridCell, storageSection);
        }

        cellStorage.AddParticleToStorageSection(storageSection, ele.second.particleIndex.value,
                                                ele.second.particleGridCell);
    }


}

void FluidSolver::CompactHashingNeighborhoodSearch::UpdateDataStructure() {
    if (initStructure || particleCollectionIndicesChangedCounter != particleCollection->GetIndicesChangedCounter()) {
        RegenerateAllDataStructures(); // use this on init and if particle collection size changed or indices of particles in the collection changed (due to sorting).
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

    size_t storageSection;
    if (!hashTable.GetValueByKey(gridCell, storageSection)) {
        return; // no entry in the hash map for this key
    }

    auto begin = cellStorage.GetStorageSectionDataBegin(storageSection);
    auto end = cellStorage.GetStorageSectionDataEnd(storageSection);
    for (auto current = begin; current != end; current++) {

        auto neighborIndex = (*current).particleIndex.value;
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

std::ostream &FluidSolver::CompactHashingNeighborhoodSearch::PrintToStream(std::ostream &os) const {

    os << "Compact Hashing Neighborhood Search - Current State" << std::endl
       << std::endl
       << "Hash Table:" << std::endl
       << hashTable << std::endl
       << "Cell Storage:" <<  std::endl
       << cellStorage << std::endl;

    return os;
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


FluidSolver::CompactHashingNeighborhoodSearch::HashTable::hash_t
FluidSolver::CompactHashingNeighborhoodSearch::HashTable::CalculateHashValue(
        const FluidSolver::CompactHashingNeighborhoodSearch::HashTable::key_t &gridCell) {
    const int32_t primeOne = 73856093;
    const int32_t primeTwo = 19349663;
    // const size_t primeThree = 83492791; // only needed for 3d
    return ((uint64_t) (gridCell.x * primeOne) ^ (uint64_t) (gridCell.y * primeTwo)) % hashTableSize;
}

bool FluidSolver::CompactHashingNeighborhoodSearch::HashTable::GetValueByKey(
        const FluidSolver::CompactHashingNeighborhoodSearch::HashTable::key_t &gridCell,
        FluidSolver::CompactHashingNeighborhoodSearch::HashTable::mappedTo_t &value) {
    auto hash = CalculateHashValue(gridCell);
    return GetValueByKeyInternal(hash, gridCell, value);
}

bool FluidSolver::CompactHashingNeighborhoodSearch::HashTable::GetValueByKeyInternal(
        FluidSolver::CompactHashingNeighborhoodSearch::HashTable::hash_t hashValue,
        const FluidSolver::CompactHashingNeighborhoodSearch::HashTable::key_t &gridCell,
        FluidSolver::CompactHashingNeighborhoodSearch::HashTable::mappedTo_t &value) {
    auto handle = hashTable[hashValue];
    if (handle.info.attributes.hasAValue == 1) {
        // check if this is the correct hash handle
        if (handle.gridCellUsingThis == gridCell) {
            value = handle.storageSectionMappedTo;
            return true;
        } else {
            // check if there was a hash collision
            if (handle.info.attributes.hashCollisionHappened == 0) {
                // there was no collision, hence there is no entry for the given key
                return false;
            } else {
                // there was a collision, check further
                hashValue = (hashValue + handle.info.attributes.relativeHashCollisionNextEntry) % hashTableSize;
                return GetValueByKeyInternal(hashValue, gridCell, value);
            }
        }
    }
    return false;
}

void FluidSolver::CompactHashingNeighborhoodSearch::HashTable::SetValueByKey(
        const FluidSolver::CompactHashingNeighborhoodSearch::HashTable::key_t &gridCell,
        const FluidSolver::CompactHashingNeighborhoodSearch::HashTable::mappedTo_t value) {
    auto hash = CalculateHashValue(gridCell);
    SetValueByKeyInternal(hash, gridCell, value);
}

void FluidSolver::CompactHashingNeighborhoodSearch::HashTable::SetValueByKeyInternal(
        FluidSolver::CompactHashingNeighborhoodSearch::HashTable::hash_t hashValue,
        const FluidSolver::CompactHashingNeighborhoodSearch::HashTable::key_t &gridCell,
        const FluidSolver::CompactHashingNeighborhoodSearch::HashTable::mappedTo_t value) {
    auto &handle = hashTable[hashValue];
    if (handle.info.attributes.hasAValue == 0) {
        // handle is empty, reserve it and set the value
        handle.info.attributes.hasAValue = 1;
        handle.gridCellUsingThis = gridCell;
        handle.storageSectionMappedTo = value;
    } else {
        // handle is already in use
        if (handle.gridCellUsingThis == gridCell) {
            // the handle is used by this key, set the value
            handle.storageSectionMappedTo = value;
        } else {
            // the handle is used by another key, check if there was a hash collision
            if (handle.info.attributes.hashCollisionHappened == 1) {
                // there was a hash collision, check the next handle
                hashValue = (hashValue + handle.info.attributes.relativeHashCollisionNextEntry) % hashTableSize;
                SetValueByKeyInternal(hashValue, gridCell, value);
            } else {
                // there was no hash collision, but this entry created one
                // find a new empty cell
                bool foundEmptyCell = false;
                handle.info.attributes.relativeHashCollisionNextEntry = -1; // setting it to maximum possible value
                for (size_t i = 1; i <= handle.info.attributes.relativeHashCollisionNextEntry; i++) {
                    hashValue = (hashValue + i) % hashTableSize;
                    auto &newHandle = hashTable[hashValue];
                    if (newHandle.info.attributes.hasAValue == 0) {
                        // we found a empty cell at index i, populate it with our data
                        SetValueByKeyInternal(hashValue, gridCell, value);
                        handle.info.attributes.relativeHashCollisionNextEntry = i; // set the appropriate hash collision flags
                        handle.info.attributes.hashCollisionHappened = 1;
                        foundEmptyCell = true;
                        break;
                    }

                }
                if (!foundEmptyCell)
                    throw std::logic_error("Could not find a empty hash table cell!");
            }
        }
    }

}

FluidSolver::CompactHashingNeighborhoodSearch::HashTable::HashTable(size_t hashTableSize) : hashTableSize(
        hashTableSize) {
    auto emptyHashTableEntry = HashTableHandle();
    emptyHashTableEntry.info.value = 0;
    hashTable.resize(hashTableSize, emptyHashTableEntry);
}

FluidSolver::CompactHashingNeighborhoodSearch::HashTable::HashTableIterator
FluidSolver::CompactHashingNeighborhoodSearch::HashTable::end() {
    return FluidSolver::CompactHashingNeighborhoodSearch::HashTable::HashTableIterator(this->hashTable.size(), this);
}

FluidSolver::CompactHashingNeighborhoodSearch::HashTable::HashTableIterator
FluidSolver::CompactHashingNeighborhoodSearch::HashTable::begin() {
    size_t firstIndex = 0;
    while (firstIndex < hashTable.size() && hashTable[firstIndex].info.attributes.hasAValue == 0) {
        firstIndex++;
    }
    return FluidSolver::CompactHashingNeighborhoodSearch::HashTable::HashTableIterator(firstIndex, this);
}

void FluidSolver::CompactHashingNeighborhoodSearch::HashTable::RemoveKey(
        const FluidSolver::CompactHashingNeighborhoodSearch::HashTable::key_t &gridCell) {
    auto hash = CalculateHashValue(gridCell);
    RemoveKeyInternal(hash, gridCell);
}

void FluidSolver::CompactHashingNeighborhoodSearch::HashTable::RemoveKeyInternal(hash_t hashValue,
                                                                                 const FluidSolver::CompactHashingNeighborhoodSearch::HashTable::key_t &gridCell) {
    auto &handle = hashTable[hashValue];
    if (handle.info.attributes.hasAValue == 0)
        return; // this key does not exist

    // the key is eventually existing, find it
    if (handle.gridCellUsingThis == gridCell) {
        // this is the handle to delete, before deleting, check if there are linked handles
        if (handle.info.attributes.hashCollisionHappened == 1) {
            // there is a link, move every link one step to us, replacing this one to 'delete' it
            auto hashOfHandleToReplace = hashValue;
            auto hashOfReplacement =
                    (hashValue + handle.info.attributes.relativeHashCollisionNextEntry) % hashTableSize;
            auto previousDiff = handle.info.attributes.relativeHashCollisionNextEntry;
            hashTable[hashOfHandleToReplace] = hashTable[hashOfReplacement];
            hashTable[hashOfHandleToReplace].info.attributes.relativeHashCollisionNextEntry = previousDiff;
            while (hashTable[hashOfReplacement].info.attributes.hashCollisionHappened == 1) {
                hashOfHandleToReplace = hashOfReplacement;
                hashOfReplacement = (hashOfReplacement +
                                     hashTable[hashOfReplacement].info.attributes.relativeHashCollisionNextEntry) %
                                    hashTableSize;
                auto previousDiff = hashTable[hashOfHandleToReplace].info.attributes.relativeHashCollisionNextEntry;
                hashTable[hashOfHandleToReplace] = hashTable[hashOfReplacement];
                hashTable[hashOfHandleToReplace].info.attributes.relativeHashCollisionNextEntry = previousDiff;
            }

            // the last cell was moved and its place is not used anymore
            hashTable[hashOfReplacement].info.attributes.hasAValue = 0;
        } else {
            // there is nothing linked to the handle, so simply setting the hasAValue flag to zero deletes it
            handle.info.attributes.hasAValue = 0;
        }
    } else {
        // this handle is responsible for another key, check if there are linked handles
        if(handle.info.attributes.hashCollisionHappened == 1) {
            // a hash collision happened aka there exists a linked handle, try to remove the key in the linked handle
            hashValue = (hashValue + handle.info.attributes.relativeHashCollisionNextEntry) % hashTableSize;
            RemoveKeyInternal(hashValue, gridCell);
        }
    }
}

std::ostream &FluidSolver::CompactHashingNeighborhoodSearch::HashTable::PrintToStream(std::ostream &os) const {

    os << "Key\t" << "Value\t" << "Collision\t" << "Rel. Collision Next Entry" << std::endl;
    for (auto handle : hashTable) {
        if (handle.info.attributes.hasAValue == 1u) {
            os << handle.gridCellUsingThis << "\t" << handle.storageSectionMappedTo << "\t"
               << handle.info.attributes.hashCollisionHappened << "\t"
               << handle.info.attributes.relativeHashCollisionNextEntry << std::endl;
        }
    }
    return os;
}


FluidSolver::CompactHashingNeighborhoodSearch::CellStorage::CellStorageIterator::CellStorageIterator(
        FluidSolver::CompactHashingNeighborhoodSearch::CellStorage *storage, size_t originalStorageSection,
        size_t currentStorageSection, uint8_t currentStorageSectionElement) : internalCellStorage(storage),
                                                                              originalStorageSection(
                                                                                      originalStorageSection),
                                                                              currentStorageSection(
                                                                                      currentStorageSection),
                                                                              currentStorageSectionElement(
                                                                                      currentStorageSectionElement) {}

bool FluidSolver::CompactHashingNeighborhoodSearch::CellStorage::CellStorageIterator::operator==(
        const FluidSolver::CompactHashingNeighborhoodSearch::CellStorage::CellStorageIterator &other) const {
    return internalCellStorage == other.internalCellStorage && other.currentStorageSection == currentStorageSection &&
           other.originalStorageSection == originalStorageSection &&
           other.currentStorageSectionElement == currentStorageSectionElement;
}

bool FluidSolver::CompactHashingNeighborhoodSearch::CellStorage::CellStorageIterator::operator!=(
        const FluidSolver::CompactHashingNeighborhoodSearch::CellStorage::CellStorageIterator &other) const {
    return !(*this == other);
}

FluidSolver::CompactHashingNeighborhoodSearch::CellStorage::GridCellParticleHandle &
FluidSolver::CompactHashingNeighborhoodSearch::CellStorage::CellStorageIterator::operator*() {
    return *(internalCellStorage->GetStorageSectionElementsDataPtr(currentStorageSection) +
             currentStorageSectionElement);
}

FluidSolver::CompactHashingNeighborhoodSearch::CellStorage::CellStorageIterator &
FluidSolver::CompactHashingNeighborhoodSearch::CellStorage::CellStorageIterator::operator++() {
    currentStorageSectionElement++;
    auto &header = internalCellStorage->GetStorageSectionHeader(currentStorageSection);
    if (currentStorageSectionElement >= header.particleIndex.internal.count &&
        header.particleIndex.internal.relativeLink != 0) {
        // we went over the end of the current storage section, but there is a linked one -> jump to the linked one
        currentStorageSectionElement = 0;
        currentStorageSection = currentStorageSection + header.particleIndex.internal.relativeLink;
    }
    return *this;
}

const FluidSolver::CompactHashingNeighborhoodSearch::CellStorage::CellStorageIterator
FluidSolver::CompactHashingNeighborhoodSearch::CellStorage::CellStorageIterator::operator++(int) {
    CellStorageIterator clone(*this);
    ++(*this);
    return clone;
}

FluidSolver::CompactHashingNeighborhoodSearch::CellStorage::GridCellParticleHandle *
FluidSolver::CompactHashingNeighborhoodSearch::CellStorage::GetStorageSectionElementsDataPtr(size_t storageSection) {
    return &data[storageSection * oneSectionTotalSize + 1];
}

FluidSolver::CompactHashingNeighborhoodSearch::CellStorage::GridCellParticleHandle &
FluidSolver::CompactHashingNeighborhoodSearch::CellStorage::GetStorageSectionHeader(size_t storageSection) {
    return data[storageSection * oneSectionTotalSize];
}

void FluidSolver::CompactHashingNeighborhoodSearch::CellStorage::ClearStorage() {

    auto resetData = GridCellParticleHandle();
    resetData.particleIndex.internal.relativeLink = 0;
    resetData.particleIndex.internal.count = 0;

    for (size_t i = 0; i < data.size() / oneSectionTotalSize; i++) {
        data[i * oneSectionTotalSize] = resetData;
    }
}

size_t FluidSolver::CompactHashingNeighborhoodSearch::CellStorage::GetEmptyStorageSection() {
    return GetEmptyStorageSection(0);
}

FluidSolver::particleAmount_t
FluidSolver::CompactHashingNeighborhoodSearch::CellStorage::GetStorageSectionElementCount(size_t storageSection) {

    auto &header = GetStorageSectionHeader(storageSection);
    if (header.particleIndex.internal.relativeLink != 0) {
        return header.particleIndex.internal.count +
               GetStorageSectionElementCount(storageSection + header.particleIndex.internal.relativeLink);
    }
    return header.particleIndex.internal.count;

}

FluidSolver::CompactHashingNeighborhoodSearch::GridCell
FluidSolver::CompactHashingNeighborhoodSearch::CellStorage::GetStorageSectionGridCell(size_t storageSection) {
    auto &header = GetStorageSectionHeader(storageSection);
    return header.particleGridCell;
}

FluidSolver::CompactHashingNeighborhoodSearch::CellStorage::CellStorageIterator
FluidSolver::CompactHashingNeighborhoodSearch::CellStorage::GetStorageSectionDataBegin(size_t storageSection) {
    return FluidSolver::CompactHashingNeighborhoodSearch::CellStorage::CellStorageIterator(this, storageSection,
                                                                                           storageSection, 0);
}

FluidSolver::CompactHashingNeighborhoodSearch::CellStorage::CellStorageIterator
FluidSolver::CompactHashingNeighborhoodSearch::CellStorage::GetStorageSectionDataEnd(size_t storageSection) {

    auto endStorageSection = storageSection;
    uint8_t endStorageSectionCount = 0;

    auto &header = GetStorageSectionHeader(endStorageSection);
    while (header.particleIndex.internal.relativeLink != 0) {
        // go through the linked storage section until we find the end
        endStorageSection = endStorageSection + header.particleIndex.internal.relativeLink;
        header = GetStorageSectionHeader(endStorageSection);
    }

    endStorageSectionCount = header.particleIndex.internal.count;
    return FluidSolver::CompactHashingNeighborhoodSearch::CellStorage::CellStorageIterator(this, storageSection,
                                                                                           endStorageSection,
                                                                                           endStorageSectionCount);
}

void FluidSolver::CompactHashingNeighborhoodSearch::CellStorage::AddParticleToStorageSection(size_t storageSection,
                                                                                             FluidSolver::particleIndex_t particleIndex,
                                                                                             const GridCell &gridCell) {
    auto &header = GetStorageSectionHeader(storageSection);
    if (header.particleIndex.internal.count < oneSectionParticleSize) {
        // the particle can be put into this storage cell
        auto particleData = GridCellParticleHandle();
        particleData.particleIndex.value = particleIndex;
        particleData.particleGridCell = gridCell;

        data[storageSection * oneSectionTotalSize + 1 + header.particleIndex.internal.count] = particleData;
        header.particleIndex.internal.count = header.particleIndex.internal.count + 1;
    } else {
        // this section is already filled up
        if (header.particleIndex.internal.relativeLink != 0) {
            // there exists already a link to another storage cell, insert the particle there
            AddParticleToStorageSection(storageSection + header.particleIndex.internal.relativeLink, particleIndex,
                                        gridCell);
        } else {
            // there is no linked storage cell existing, find an empty one after this one and use it
            auto newEmptyStorageSection = GetEmptyStorageSection(storageSection + 1);
            auto &newHeader = GetStorageSectionHeader(newEmptyStorageSection);
            newHeader.particleGridCell = gridCell;
            AddParticleToStorageSection(newEmptyStorageSection, particleIndex, gridCell);
        }
    }
}

size_t
FluidSolver::CompactHashingNeighborhoodSearch::CellStorage::GetEmptyStorageSection(size_t minimumStorageSectionValue) {
    // TODO: this returns only 1
    for (size_t i = minimumStorageSectionValue; i < data.size() / oneSectionTotalSize; i++) {
        auto &header = GetStorageSectionHeader(i);
        if (header.particleIndex.internal.count == 0) {
            return i;
        }
    }

    auto resetData = GridCellParticleHandle();
    resetData.particleIndex.internal.relativeLink = 0;
    resetData.particleIndex.internal.count = 0;

    auto newStorageSection = data.size() / oneSectionTotalSize;
    data.resize(data.size() + oneSectionTotalSize, resetData);
    return newStorageSection;
}

void FluidSolver::CompactHashingNeighborhoodSearch::CellStorage::RemoveParticleFromStorageSection(size_t storageSection,
                                                                                                  FluidSolver::particleIndex_t particleIndex) {
    RemoveParticleFromStorageSectionInternal(storageSection, storageSection, particleIndex);
}

void FluidSolver::CompactHashingNeighborhoodSearch::CellStorage::RemoveParticleFromStorageSectionInternal(
        size_t storageSection, size_t storageSectionBefore, FluidSolver::particleIndex_t particleIndex) {

    auto &header = GetStorageSectionHeader(storageSection);

    if (header.particleIndex.internal.relativeLink != 0) {
        RemoveParticleFromStorageSectionInternal(storageSection + header.particleIndex.internal.relativeLink,
                                                 storageSection, particleIndex);
    }

    for (size_t i = header.particleIndex.internal.count; i >= 1; i--) {
        auto index = storageSection * oneSectionTotalSize + i;
        if (data[index].particleIndex.value == particleIndex) {
            // delete this one by either moving the last entry here (or if this is the last entry, reducing the count of this section by one)
            if (i == header.particleIndex.internal.count) {
                // this is the last element
                if (header.particleIndex.internal.relativeLink != 0) {
                    // there exists a link, so we can replace the last one with an extracted one from the link
                    data[index] = ExtractLastOne(storageSection + header.particleIndex.internal.relativeLink,
                                                 storageSection);
                } else {
                    // there is nothing to replace this entry, simply delete it by reducing the count
                    header.particleIndex.internal.count -= 1;
                }
            } else {
                // the particle is somewhere in the middle, just take the last particle and replace the particle with that
                data[index] = ExtractLastOne(storageSection, storageSectionBefore);
            }

        }
    }


}

FluidSolver::CompactHashingNeighborhoodSearch::CellStorage::GridCellParticleHandle
FluidSolver::CompactHashingNeighborhoodSearch::CellStorage::ExtractLastOne(size_t storageSection,
                                                                           size_t storageSectionBefore) {
    auto &header = GetStorageSectionHeader(storageSection);
    if (header.particleIndex.internal.relativeLink != 0) {
        return ExtractLastOne(storageSection + header.particleIndex.internal.count, storageSection);
    } else {
        GridCellParticleHandle extracted = data[storageSection * oneSectionTotalSize +
                                                header.particleIndex.internal.count];
        header.particleIndex.internal.count = header.particleIndex.internal.count - 1;
        if (header.particleIndex.internal.count == 0) {
            // the storage section became empty, remove the link to this section in the previous storage section
            if (storageSection != storageSectionBefore) {
                // but only if the storage sections are different
                auto &prevHeader = GetStorageSectionHeader(storageSectionBefore);
                prevHeader.particleIndex.internal.relativeLink = 0;
            }
        }
        return extracted;
    }
}

FluidSolver::CompactHashingNeighborhoodSearch::CellStorage::CellStorage(uint8_t oneSectionParticleSize)
        : oneSectionParticleSize(oneSectionParticleSize) {
    oneSectionTotalSize = oneSectionParticleSize + 1;
}

std::ostream &FluidSolver::CompactHashingNeighborhoodSearch::CellStorage::PrintToStream(std::ostream &os) const {
    return os;
}


FluidSolver::CompactHashingNeighborhoodSearch::HashTable::HashTableIterator::HashTableIterator(size_t currentIndex,
                                                                                               FluidSolver::CompactHashingNeighborhoodSearch::HashTable *table)
        : currentIndex(currentIndex), table(table) {}

bool FluidSolver::CompactHashingNeighborhoodSearch::HashTable::HashTableIterator::operator==(
        const FluidSolver::CompactHashingNeighborhoodSearch::HashTable::HashTableIterator &other) const {
    return this->table == other.table && this->currentIndex == other.currentIndex;
}

bool FluidSolver::CompactHashingNeighborhoodSearch::HashTable::HashTableIterator::operator!=(
        const FluidSolver::CompactHashingNeighborhoodSearch::HashTable::HashTableIterator &other) const {
    return !(*this == other);
}

FluidSolver::CompactHashingNeighborhoodSearch::HashTable::key_t &
FluidSolver::CompactHashingNeighborhoodSearch::HashTable::HashTableIterator::operator*() {
    return table->hashTable[this->currentIndex].gridCellUsingThis;
}

FluidSolver::CompactHashingNeighborhoodSearch::HashTable::HashTableIterator &
FluidSolver::CompactHashingNeighborhoodSearch::HashTable::HashTableIterator::operator++() {
    this->currentIndex++;
    while (this->currentIndex < table->hashTable.size() &&
           table->hashTable[currentIndex].info.attributes.hasAValue == 0) {
        this->currentIndex++;
    }
    return *this;
}

const FluidSolver::CompactHashingNeighborhoodSearch::HashTable::HashTableIterator
FluidSolver::CompactHashingNeighborhoodSearch::HashTable::HashTableIterator::operator++(int) {
    HashTableIterator clone(*this);
    ++(*this);
    return clone;
}


std::ostream &operator<<(std::ostream &os, FluidSolver::CompactHashingNeighborhoodSearch::HashTable const &m) {
    return m.PrintToStream(os);
}

std::ostream &operator<<(std::ostream &os, FluidSolver::CompactHashingNeighborhoodSearch::CellStorage const &m) {
    return m.PrintToStream(os);
}

std::ostream &operator<<(std::ostream &os, FluidSolver::CompactHashingNeighborhoodSearch const &m) {
    return m.PrintToStream(os);
}

std::ostream &operator<<(std::ostream &os, FluidSolver::CompactHashingNeighborhoodSearch::GridCell const &m) {
    os << "{ x=" << m.x << ", y=" << m.y << " }";
    return os;
}