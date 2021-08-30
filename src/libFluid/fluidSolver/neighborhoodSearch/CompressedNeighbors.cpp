#include "CompressedNeighbors.hpp"

#include "fluidSolver/ParticleCollectionAlgorithm.hpp"
#include "parallelization/StdParallelForEach.hpp"

#include <array>
#include <libmorton/morton.h>

namespace FluidSolver
{

    using parallel = StdParallelForEach;


    Compatibility CompressedNeighborhoodSearch::check()
    {
        Compatibility c;
        if (collection == nullptr)
        {
            c.add_issue({FLUID_NAMEOF(CompressedNeighborhoodSearch), "ParticleCollection is null."});
        }
        else
        {
            if (!collection->is_type_present<MovementData3D>())
            {
                c.add_issue({FLUID_NAMEOF(CompressedNeighborhoodSearch),
                             "Particles are missing the MovementData3D attribute."});
            }
            if (!collection->is_type_present<ParticleInfo>())
            {
                c.add_issue(
                    {FLUID_NAMEOF(CompressedNeighborhoodSearch), "Particles are missing the ParticleInfo attribute."});
            }
        }

        if (search_radius <= 0.0f)
        {
            c.add_issue({FLUID_NAMEOF(CompressedNeighborhoodSearch), "Search radius is smaller or equal to zero."});
        }

        return c;
    }

    CompressedNeighborhoodSearch::GridCellLocation CompressedNeighborhoodSearch::
        calculate_grid_cell_location_of_position(const vec3& position)
    {
        return {(int)std::floor(position.x / search_radius), (int)std::floor(position.y / search_radius),
                (int)std::floor(position.z / search_radius)};
    }

    uint64_t CompressedNeighborhoodSearch::calculate_cell_index_by_cell_location(const GridCellLocation& location)
    {
        auto transform_into_unsigned = [](int32_t v) -> uint32_t {
            int64_t v2 = (int64_t)v;
            v2 -= std::numeric_limits<int32_t>::min();
            uint32_t res = (uint32_t)v2;
            return res;
        };


        uint64_t key =
            libmorton::morton3D_64_encode(transform_into_unsigned(location.x), transform_into_unsigned(location.y),
                                          transform_into_unsigned(location.z));
        return key;
    }

    size_t CompressedNeighborhoodSearch::get_particle_index_by_cell_index(size_t cell_index) const
    {
        std::function<size_t(size_t, size_t)> recursive_search = [&](size_t left, size_t right) -> size_t {
            if (left >= cell_to_particle_map.size() || right == (size_t)(-1))
            {
                // could not find entry
                return -1;
            }

            size_t middle = (right - left) / 2 + left;
            FLUID_ASSERT(middle < cell_to_particle_map.size());

            if (cell_to_particle_map[middle].cell_index > cell_index)
            {
                // search in the left part
                return recursive_search(left, middle);
            }
            else if (cell_to_particle_map[middle].cell_index < cell_index)
            {
                // search in the right part
                return recursive_search(middle + 1, right);
            }
            else
            {
                // we found the cell index
                return cell_to_particle_map[middle].index_of_first_particle;
            }
        };

        return recursive_search(0, cell_to_particle_map.size());
    }

    void CompressedNeighborhoodSearch::initialize()
    {
        if (!collection->is_type_present<ParticleInformation>())
        {
            collection->add_type<ParticleInformation>();
        }
        if (!collection->is_type_present<NeighborStorage>())
        {
            collection->add_type<NeighborStorage>();
        }
    }

    void CompressedNeighborhoodSearch::find_neighbors()
    {
        FLUID_ASSERT(collection != nullptr);
        FLUID_ASSERT(collection->is_type_present<ParticleInformation>());
        FLUID_ASSERT(collection->is_type_present<NeighborStorage>());

        // calculate grid cell and cell index of each particle
        parallel::loop_for(0, collection->size(), [&](size_t particle_index) {
            const auto& particle_info = collection->get<ParticleInfo>(particle_index);
            auto& information = collection->get<ParticleInformation>(particle_index);
            if (particle_info.type == ParticleTypeDead)
            {
                information.cell_index = -1;
                information.first_particle_of_cell = false;
                information.cell_location = GridCellLocation::undefined();
                return;
            }

            const auto& mv = collection->get<MovementData3D>(particle_index);

            information.first_particle_of_cell = false;
            information.cell_location = calculate_grid_cell_location_of_position(mv.position);
            information.cell_index = calculate_cell_index_by_cell_location(information.cell_location);
        });

        // sort particles according to cell index
        {
            ParticleCollectionAlgorithm::Sort sorter;
            sorter.adapt_collection(collection);

            sorter.quick_sort(
                collection,
                [&](const std::shared_ptr<ParticleCollection>& collection, const pIndex_t index) -> uint64_t {
                    auto& information = collection->get<ParticleInformation>(index);
                    return information.cell_index;
                });
        }

        // build map that maps cell to cell start
        {
            cell_to_particle_map.clear();
            for (size_t particle_index = 0; particle_index < collection->size(); particle_index++)
            {
                const auto& particle_info = collection->get<ParticleInfo>(particle_index);
                if (particle_info.type == ParticleTypeDead)
                {
                    break;
                }

                auto& information = collection->get<ParticleInformation>(particle_index);
                if (particle_index == 0)
                {
                    information.first_particle_of_cell = true;
                    cell_to_particle_map.push_back({information.cell_location, particle_index, information.cell_index});
                    continue;
                }

                const auto& prev_information = collection->get<ParticleInformation>(particle_index - 1);
                if (prev_information.cell_index != information.cell_index)
                {
                    information.first_particle_of_cell = true;
                    cell_to_particle_map.push_back({information.cell_location, particle_index, information.cell_index});
                }
            }
        }

        // find neighbors of each particle
        {
            parallel::loop_for(0, collection->size(), [&](size_t particle_index) {
                const auto& pi = collection->get<ParticleInfo>(particle_index);
                if (pi.type == ParticleTypeDead)
                {
                    return;
                }

                const auto& information = collection->get<ParticleInformation>(particle_index);

                std::array<size_t, 27> cell_indices_to_check;
                size_t i = 0;
                for (int x = -1; x <= 1; x++)
                {
                    for (int y = -1; y <= 1; y++)
                    {
                        for (int z = -1; z <= 1; z++)
                        {
                            FLUID_ASSERT(i < cell_indices_to_check.size());

                            cell_indices_to_check[i] = calculate_cell_index_by_cell_location({
                                information.cell_location.x + x,
                                information.cell_location.y + y,
                                information.cell_location.z + z,
                            });
                            i++;
                        }
                    }
                }
                std::sort(cell_indices_to_check.begin(), cell_indices_to_check.end());

                auto& storage = collection->get<NeighborStorage>(particle_index);
                storage.clear();
                size_t last_neighbor = -1;
                size_t delta_counter = 0;

                const auto& mv_particle = collection->get<MovementData3D>(particle_index);

                for (size_t i = 0; i < cell_indices_to_check.size(); i++)
                {
                    size_t first_particle_index = get_particle_index_by_cell_index(cell_indices_to_check[i]);
                    if (first_particle_index == (size_t)(-1))
                    {
                        // the cell is empty and therefore non existant
                        continue;
                    }

                    size_t current_particle = first_particle_index;
                    while (cell_indices_to_check[i] ==
                           collection->get<ParticleInformation>(current_particle).cell_index)
                    {
                        const auto& mv_current = collection->get<MovementData3D>(current_particle);
                        auto diff = mv_current.position - mv_particle.position;
                        if (glm::dot(diff, diff) <= Math::pow2(search_radius))
                        {
                            // the particles are neighbors
                            if (last_neighbor == (size_t)(-1))
                            {
                                last_neighbor = current_particle;
                                storage.set_first_neighbor(last_neighbor);
                            }
                            else
                            {
                                FLUID_ASSERT(last_neighbor < current_particle);
                                size_t delta = current_particle - last_neighbor;
                                storage.set_next_neighbor(delta, delta_counter);
                                last_neighbor = current_particle;
                                delta_counter++;
                            }
                        }
                        current_particle++;
                        if (current_particle >= collection->size())
                        {
                            break;
                        }
                    }
                }
            });
        }
    }


    void CompressedNeighborhoodSearch::NeighborStorage::clear()
    {
        first_neighbor = -1;
        for (size_t i = 0; i < MAX_DELTAS; i++)
        {
            deltas[i] = 0;
        }
    }

    void CompressedNeighborhoodSearch::NeighborStorage::set_first_neighbor(size_t index)
    {
        first_neighbor = index;
    }

    void CompressedNeighborhoodSearch::NeighborStorage::set_next_neighbor(size_t delta_to_previous_neighbor,
                                                                          size_t neighbor_delta_counter)
    {
        FLUID_ASSERT(neighbor_delta_counter < MAX_DELTAS);
        deltas[neighbor_delta_counter] = delta_to_previous_neighbor;
    }

} // namespace FluidSolver
