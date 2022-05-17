#pragma once

#include "Compatibility.hpp"
#include "FluidInclude.hpp"
#include "fluidSolver/ParticleCollection.hpp"
#include "fluidSolver/neighborhoodSearch/NeighborhoodInterface.hpp"
#include "helper/ProtectedUnorderedMap.hpp"

#include <array>
#include <limits>
#include <memory>
#include <set>
#include <optional>

namespace FluidSolver
{


    class HashedNeighborhoodSearch3D {
      public:
        using particleIndex_t = pIndex_t;
        using particleAmount_t = uint16_t;


        struct Neighbors;

        struct NeighborsIterator
        {

            const Neighbors* data;
            particleIndex_t current;

            int8_t dx = -1;
            int8_t dy = -1;
            int8_t dz = -1;

            std::optional<std::set<particleIndex_t>::iterator> current_set_iterator;

            bool operator==(const NeighborsIterator& other) const;

            bool operator!=(const NeighborsIterator& other) const;

            particleIndex_t& operator*();

            NeighborsIterator& operator++();

            const NeighborsIterator operator++(int);
        };

        struct Neighbors
        {

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
                vec3 position;
                particleIndex_t particle;
            } of = {};
            bool position_based = false;
            HashedNeighborhoodSearch3D* data = nullptr;

            NeighborsIterator begin() const;

            NeighborsIterator end() const;
        };

        std::shared_ptr<ParticleCollection> collection = nullptr;
        pFloat search_radius = 0.0f;

        void find_neighbors();

        Neighbors get_neighbors(particleIndex_t particleIndex);

        Neighbors get_neighbors(const vec3& position);

        void initialize();

        std::shared_ptr<NeighborhoodInterface> create_interface();

        Compatibility check();

      private:
        // Grid data



        struct GridCellLocation
        {
            int x = std::numeric_limits<int>::min();
            int y = std::numeric_limits<int>::min();
            int z = std::numeric_limits<int>::min();

            bool operator==(const GridCellLocation& other) const;
            bool operator!=(const GridCellLocation& other) const;

            struct hash
            {
                inline size_t operator()(const GridCellLocation& cell) const
                {
                    return std::hash<int>()(cell.x) ^ std::hash<int>()(cell.y) ^ std::hash<int>()(cell.z);
                }
            };

            static constexpr GridCellLocation undefined()
            {
                return {std::numeric_limits<int>::min(), std::numeric_limits<int>::min(),
                        std::numeric_limits<int>::min()};
            }
        };

        struct GridCellState
        {
            GridCellLocation previous;
            GridCellLocation current;
        };

        GridCellLocation calculate_grid_cell_location_of_particle(particleIndex_t index);
        GridCellLocation calculate_grid_cell_location_of_position(const vec3& position);


        void rebuild_grid();

        void update_grid();

        void find_neighbors_with_grid();

        Helper::ProtectedUnorderedMap<GridCellLocation, std::set<particleIndex_t>, GridCellLocation::hash> grid;
        bool grid_rebuild_required = true;


      private:
        // Neighbor data


        struct NeighborData
        {
            size_t size;
            std::vector<particleIndex_t> neighbor_indices;
        };


        std::vector<NeighborData> neighbor_data;

        private:

            size_t calls_since_last_cache_efficiency_improvement = 0;

            void improve_cache_efficiency();
    };


} // namespace FluidSolver