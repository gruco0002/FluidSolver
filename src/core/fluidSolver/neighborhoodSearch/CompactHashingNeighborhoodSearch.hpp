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

        void RegenerateAllDataStructures();

        void SearchByDifference();

        size_t particleCollectionIndicesChangedCounter;
        bool initStructure;

        struct GridCell {
            int32_t x = 0;
            int32_t y = 0;

            bool operator==(const GridCell &rhs) const;

            bool operator!=(const GridCell &rhs) const;

            GridCell() = default;

            GridCell(int32_t x, int32_t y);
        };


        class HashTable {
        public:

            typedef size_t mappedTo_t;
            typedef GridCell key_t;
            typedef size_t hash_t;

            class HashTableIterator {
            public:
                HashTableIterator(size_t currentIndex, HashTable *table);

                bool operator==(const HashTableIterator &other) const;

                bool operator!=(const HashTableIterator &other) const;

                key_t &operator*();

                HashTableIterator &operator++();

                const HashTableIterator operator++(int);

            private:
                size_t currentIndex;
                HashTable *table;
            };


            explicit HashTable(size_t hashTableSize);

            bool GetValueByKey(const key_t &gridCell, mappedTo_t &value);

            void RemoveKey(const key_t &gridCell);

            void SetValueByKey(const key_t &gridCell, mappedTo_t value);

            HashTableIterator begin();
            HashTableIterator end();

        private:


            size_t hashTableSize = 0;

            struct HashTableHandle {
                mappedTo_t storageSectionMappedTo = 0;
                key_t gridCellUsingThis;
                union {
                    struct {
                        unsigned hashCollisionHappened : 1;
                        unsigned hasAValue : 1;
                        unsigned relativeHashCollisionNextEntry : 6;
                    } attributes;
                    uint8_t value = 0;
                } info;
            };

            std::vector<HashTableHandle> hashTable;

            hash_t CalculateHashValue(const key_t &gridCell);


            bool GetValueByKeyInternal(hash_t hashValue, const key_t &gridCell, mappedTo_t &value);

            void SetValueByKeyInternal(hash_t hashValue, const key_t &gridCell, const mappedTo_t value);

            void RemoveKeyInternal(hash_t hashValue, const key_t &gridCell);

        };

        float cellSize;
        HashTable hashTable;

        class CellStorage {

        public:
            struct GridCellParticleHandle {
                union {
                    particleIndex_t value = 0;
                    struct {
                        unsigned count : 8;
                        unsigned relativeLink : 24;
                    } internal;
                } particleIndex;

                GridCell particleGridCell;
            };

            class CellStorageIterator {

            public:

                CellStorageIterator(CellStorage *storage, size_t originalStorageSection, size_t currentStorageSection,
                                    uint8_t currentStorageSectionElement);

                bool operator==(const CellStorageIterator &other) const;

                bool operator!=(const CellStorageIterator &other) const;

                GridCellParticleHandle &operator*();

                CellStorageIterator &operator++();

                const CellStorageIterator operator++(int);

            private:

                CellStorage *internalCellStorage;
                size_t originalStorageSection;
                size_t currentStorageSection;
                uint8_t currentStorageSectionElement;

            };


            void
            AddParticleToStorageSection(size_t storageSection, particleIndex_t particleIndex, const GridCell &gridCell);

            void RemoveParticleFromStorageSection(size_t storageSection, particleIndex_t particleIndex);

            size_t GetEmptyStorageSection();

            particleAmount_t GetStorageSectionElementCount(size_t storageSection);

            GridCell GetStorageSectionGridCell(size_t storageSection);

            CellStorageIterator GetStorageSectionDataBegin(size_t storageSection);

            CellStorageIterator GetStorageSectionDataEnd(size_t storageSection);

            void ClearStorage();

            CellStorage(uint8_t oneSectionParticleSize);

        private:

            size_t GetEmptyStorageSection(size_t minimumStorageSectionValue);

            // points towards the first element (not the header) of the storage section
            GridCellParticleHandle *GetStorageSectionElementsDataPtr(size_t storageSection);

            GridCellParticleHandle &GetStorageSectionHeader(size_t storageSection);

            void RemoveParticleFromStorageSectionInternal(size_t storageSection, size_t storageSectionBefore,
                                                          particleIndex_t particleIndex);

            GridCellParticleHandle ExtractLastOne(size_t storageSection, size_t storageSectionBefore);

            uint8_t oneSectionParticleSize;
            uint8_t oneSectionTotalSize;

            /**
             * The size of the storage is always a multiple of oneSectionTotalSize.
             * The section storage is divided into sections of the oneSectionTotalSize size.
             * The first element of each section is called the header and its particle index represents the internal data like the count and the link to another section.
             * The particleGridCell of the header represents the grid cell this storage is responsible for.
             * The remaining oneSectionParticleSize entries represent the actual particles of this storage cell, but only
             * as many as the header count variable states. The particleGridCell variable of these particles is used whilst
             * updating the structure. At any other times this fields data should not be read as it would eventually
             * represent wrong data.
             */
            std::vector<GridCellParticleHandle> data;


        };

        CellStorage cellStorage;

        GridCell CalculateCellCoordinates(particleIndex_t particleIndex);

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
