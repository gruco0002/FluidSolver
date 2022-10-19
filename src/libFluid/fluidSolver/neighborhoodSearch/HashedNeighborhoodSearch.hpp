#pragma once

#include "LibFluidTypes.hpp"
#include "fluidSolver/ParticleCollection.hpp"
#include "fluidSolver/neighborhoodSearch/NeighborhoodInterface.hpp"
#include "helpers/CompatibilityReport.hpp"
#include "helpers/Initializable.hpp"
#include "helpers/Reportable.hpp"

#include <list>
#include <memory>
#include <unordered_map>
#include <vector>

namespace LibFluid {


    class HashedNeighborhoodSearch : public Initializable, public Reportable {
      public:
        using particleIndex_t = pIndex_t;
        using particleAmount_t = uint16_t;


        struct Neighbors;

        struct NeighborsIterator {
            const Neighbors* data;
            particleIndex_t current;

            bool operator==(const NeighborsIterator& other) const;

            bool operator!=(const NeighborsIterator& other) const;

            particleIndex_t& operator*();

            NeighborsIterator& operator++();

            const NeighborsIterator operator++(int);
        };

        struct Neighbors {
            // iterator defines
            using T = particleIndex_t;
            using iterator = NeighborsIterator;
            using const_iterator = NeighborsIterator;
            using difference_type = ptrdiff_t;
            using size_type = size_t;
            using value_type = T;
            using pointer = T*;
            using const_pointer = const T*;
            using reference = T&;

            // data
            union {
                vec2 position;
                particleIndex_t particle;
            } of = {};
            bool position_based = false;
            HashedNeighborhoodSearch* data = nullptr;

            NeighborsIterator begin() const;

            NeighborsIterator end() const;
        };

        std::shared_ptr<ParticleCollection> collection = nullptr;
        pFloat search_radius = 0.0f;

        void find_neighbors();

        Neighbors get_neighbors(particleIndex_t particleIndex);

        Neighbors get_neighbors(const vec2& position);

        void initialize() override;

        std::shared_ptr<NeighborhoodInterface> create_interface();

        void create_compatibility_report(CompatibilityReport& report) override;

      private:
        pFloat grid_cell_size = 0.0f;


        typedef std::pair<int32_t, int32_t> GridKey;

        struct GridKeyHash {
            size_t operator()(GridKey p) const noexcept {
                return (size_t((uint32_t)p.first) << 32) | ((uint32_t)p.second);
            }
        };

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
        std::unordered_map<particleIndex_t, std::pair<particleAmount_t, std::vector<particleIndex_t>>> neighbors;

        particleIndex_t bucketsCreatedUntilIndex = 0;

        GridKey GetGridCellByParticleID(particleIndex_t particleIndex);

        GridKey GetGridCellByPosition(const vec2& position);

        void CreateGridEntryIfNecessary(const GridKey& key);

        void UpdateGrid();
    };


} // namespace LibFluid
