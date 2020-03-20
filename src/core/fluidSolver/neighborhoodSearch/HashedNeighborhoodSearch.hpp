#ifndef FLUIDSOLVER_HASHEDNEIGHBORHOODSEARCH_HPP
#define FLUIDSOLVER_HASHEDNEIGHBORHOODSEARCH_HPP

#include <core/fluidSolver/neighborhoodSearch/INeighborhoodSearch.hpp>
#include <unordered_map>
#include <vector>
#include <list>

namespace FluidSolver {
    class HashedNeighborhoodSearch : public INeighborhoodSearch {
    public:

        void FindNeighbors() override;

        std::shared_ptr<Neighbors> GetNeighbors(particleIndex_t particleIndex) override;

        void UpdateGrid();
        std::shared_ptr<Neighbors> GetNeighbors(glm::vec2 position) override;

        HashedNeighborhoodSearch(IParticleCollection *particleCollection, float radius);


    private:
        typedef std::pair<int32_t, int32_t> GridKey;

        struct GridKeyHash {
            //static_assert(sizeof(int32_t) * 2 == sizeof(size_t));

            size_t operator()(GridKey p) const noexcept {
                return (size_t((uint32_t) p.first) << 32) | ((uint32_t) p.second);
            }
        };

        /**
         * Can be an arbitrary value, but it is recommended to set it to 3 times the particle size.
         */
        float gridCellSize;

        /**
         * Maps the Grid cell to its containing particles
         */
        std::unordered_map<GridKey, std::list<particleIndex_t>, GridKeyHash> gridToParticles;

        /**
         * Contains the current status of each particle.
         */
        std::unordered_map<particleIndex_t, GridKey> currentStatus;

        /**
         * Map containing the neighbor count and neighbors of a given particle
         */
        std::unordered_map<particleIndex_t, std::pair<particleAmount_t, std::vector<particleIndex_t >>> neighbors;

        particleIndex_t bucketsCreatedUntilIndex = 0;

        GridKey GetGridCellByParticleID(particleIndex_t particleIndex);

        GridKey GetGridCellByPosition(glm::vec2 &position);

        void CreateGridEntryIfNecessary(GridKey &key);




    };
}

#endif //FLUIDSOLVER_HASHEDNEIGHBORHOODSEARCH_HPP
