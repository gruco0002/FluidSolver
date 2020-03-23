#ifndef FLUIDSOLVER_COMPACTHASHINGNEIGHBORHOODSEARCH_HPP
#define FLUIDSOLVER_COMPACTHASHINGNEIGHBORHOODSEARCH_HPP

#include "INeighborhoodSearch.hpp"

namespace FluidSolver {

    class CompactHashingNeighborhoodSearch : public INeighborhoodSearch {

    public:


        CompactHashingNeighborhoodSearch(IParticleCollection *particleCollection, float radius);

        void FindNeighbors() override;

        NeighborsCompact GetNeighbors(particleIndex_t particleIndex) override;

        NeighborsCompactData GetNeighbors(glm::vec2 position) override;

    private:

        void UpdateDataStructure();

        void TotalResetSearch();

        void SearchByDifference();

        size_t particleCollectionIndicesChangedCounter;
        bool initStructure;


        /**
         * Should be two times the amount of particles.
         */
        size_t hashTableSize = 0;
        float cellSize;
        size_t storageSectionSize = 51;

        const int32_t primeOne = 73856093;
        const int32_t primeTwo = 19349663;
        // const size_t primeThree = 83492791; // only needed for 3d

        struct GridCell {
            int32_t x = 0;
            int32_t y = 0;

            bool operator==(const GridCell &rhs) const;

            bool operator!=(const GridCell &rhs) const;

            GridCell() = default;

            GridCell(int32_t x, int32_t y);
        };

        struct HashTableHandle {
            size_t storageSectionMappedTo = 0;
            GridCell gridCellUsingThis;
            union {
                struct {
                    unsigned hashCollisionHappened : 1;
                    unsigned hasAValue : 1;
                    unsigned UNUSED_UNION_BITS : 6;
                } attributes;
                uint8_t value = 0;
            } info;
        };

        struct GridCellParticleHandle {
            particleIndex_t particleIndex = 0;
            GridCell particleGridCell;
        };

        /**
         * The size of the cell storage is always a multiple of storageSectionSize.
         * The cell storage is divided into cells of the storageSectionSize size.
         * The first element of each cell is called the header and its particle index represents the number of elements
         * contained in the storage section (zero if empty). The particleGridCell of the header represents the grid cell
         * this storage is responsible for.
         * The remaining storageSectionSize - 1 entries represent the actual particles of this storage cell, but only
         * as many as the header count variable states. The particleGridCell variable of these particles is used whilst
         * updating the structure. At any other times this fields data should not be read as it would eventually
         * represent wrong data.
         */
        std::vector<GridCellParticleHandle> cellStorage;


        std::vector<HashTableHandle> hashTable;

        size_t CalculateHashValue(const GridCell &gridCell);

        GridCell CalculateCellCoordinates(particleIndex_t particleIndex);


        size_t GetOrAquireStorageCell(size_t hash, const GridCell &cell);

        void AddParticleToStorageSection(size_t storageSection, particleIndex_t particleIndex);

        size_t AquireNewCellStorageSection();


        class NeighborStorage {

        public:

            NeighborStorage(particleAmount_t neighborStorageSizePerParticle, particleAmount_t totalParticleCount);

            void SetCount(particleIndex_t particleIndex, particleAmount_t count);

            particleAmount_t GetCount(particleIndex_t particleIndex);

            particleIndex_t *GetDataPtr(particleIndex_t particleIndex);

            particleAmount_t GetNeighborStorageSizePerParticle();

            void SetNeighborStorageSizePerParticle(particleAmount_t size);

            particleAmount_t getTotalParticleCount() const;

            void setTotalParticleCount(particleAmount_t totalParticleCount);

        private:
            particleAmount_t neighborStorageSizePerParticle;
            particleAmount_t totalParticleCount;

            void ResizeVector();

            std::vector<particleIndex_t> neighbors;

        };

        NeighborStorage storage;

        void FindNeighborsForCellForParticle(const glm::vec2 &particlePosition, const GridCell &gridCell,
                                             particleAmount_t &neighborCount, particleIndex_t *data);

    };

}


#endif //FLUIDSOLVER_COMPACTHASHINGNEIGHBORHOODSEARCH_HPP
