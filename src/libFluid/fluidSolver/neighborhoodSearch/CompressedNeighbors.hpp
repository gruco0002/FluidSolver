#pragma once

#include "Compatibility.hpp"
#include "FluidInclude.hpp"
#include "fluidSolver/ParticleCollection.hpp"
#include "fluidSolver/neighborhoodSearch/NeighborhoodInterface.hpp"

#include <bitset>
#include <memory>
#include <vector>

namespace FluidSolver
{


    /**
     * @brief Implementation of the compressed neighborhood search as described in
     * the paper 'Compressed Neighbour Lists for SPH' by Band et al. (doi:10.1111/cgf.13890)
     */
    class CompressedNeighborhoodSearch {

      public:
        using particleIndex_t = pIndex_t;
        using particleAmount_t = uint16_t;


        struct Neighbors;

        struct NeighborsIterator
        {

            const Neighbors* data = nullptr;
            particleIndex_t current = 0;
            size_t current_counter = 0;

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
            CompressedNeighborhoodSearch* data = nullptr;

            NeighborsIterator begin() const;

            NeighborsIterator end() const;
        };

        std::shared_ptr<ParticleCollection> collection = nullptr;
        pFloat search_radius = 0.0f;

        void find_neighbors();

        Neighbors get_neighbors(particleIndex_t particleIndex);

        Neighbors get_neighbors(const vec3& position);

        void initialize();

        NeighborhoodInterface create_interface();

        Compatibility check();


      private:
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

        GridCellLocation calculate_grid_cell_location_of_position(const vec3& position);

        static uint64_t calculate_cell_index_by_cell_location(const GridCellLocation& location);

        struct ParticleInformation
        {
            size_t cell_index;
            GridCellLocation cell_location;
            bool first_particle_of_cell;
        };

        struct GridCellToParticle
        {
            GridCellLocation cell_location;
            size_t index_of_first_particle;
            size_t cell_index;
        };

        std::vector<GridCellToParticle> cell_to_particle_map;

        size_t get_particle_index_by_cell_index(size_t cell_index) const;

      public:
        struct NeighborStorage
        {
            static constexpr size_t MAX_DELTAS = 48;
            static constexpr size_t DELTAS_SIZE = 48;
            static constexpr size_t MAX_CONTROLS = MAX_DELTAS * 2;


            void clear();
            void set_first_neighbor(size_t index);
            void set_next_neighbor(size_t delta_to_previous_neighbor);
            size_t size() const;
            uint32_t get_delta(size_t delta_index) const;
            uint32_t get_first_neighbor() const;
            size_t get_used_delta_bytes() const;

          private:
            size_t first_neighbor;
            std::bitset<MAX_CONTROLS> control_sequence;
            uint8_t deltas[DELTAS_SIZE];


            size_t size_value;
            size_t current_deltas_byte_size;
        };
    };


} // namespace FluidSolver