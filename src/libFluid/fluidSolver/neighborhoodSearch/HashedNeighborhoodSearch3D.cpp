#include "HashedNeighborhoodSearch3D.hpp"

#include "fluidSolver/ParticleCollectionAlgorithm.hpp"
#include "parallelization/StdParallelForEach.hpp"

#include <libmorton/morton.h>

namespace LibFluid {

    using parallel = StdParallelForEach;

    void HashedNeighborhoodSearch3D::initialize() {
        FLUID_ASSERT(collection != nullptr);
        if (!collection->is_type_present<GridCellState>()) {
            collection->add_type<GridCellState>();
        }
        grid_rebuild_required = true;
        grid.auto_initialize_protection_enabled = true;
    }

    void HashedNeighborhoodSearch3D::create_compatibility_report(CompatibilityReport& report) {
        report.begin_scope(FLUID_NAMEOF(HashedNeighborhoodSearch3D));
        if (collection == nullptr) {
            report.add_issue("ParticleCollection is null.");
        } else {
            if (!collection->is_type_present<MovementData3D>()) {
                report.add_issue("Particles are missing the MovementData3D attribute.");
            }
            if (!collection->is_type_present<ParticleInfo>()) {
                report.add_issue("Particles are missing the ParticleInfo attribute.");
            }
            if (!collection->is_type_present<GridCellState>()) {
                report.add_issue("Particles are missing the GridCellState attribute.");
            }
        }

        if (search_radius <= 0.0f) {
            report.add_issue("Search radius is smaller or equal to zero.");
        }

        report.end_scope();
    }

    bool HashedNeighborhoodSearch3D::GridCellLocation::operator==(const GridCellLocation& other) const {
        return x == other.x && y == other.y && z == other.z;
    }


    bool HashedNeighborhoodSearch3D::GridCellLocation::operator!=(const GridCellLocation& other) const {
        return !(*this == other);
    }

    HashedNeighborhoodSearch3D::GridCellLocation HashedNeighborhoodSearch3D::calculate_grid_cell_location_of_particle(
            particleIndex_t index) {
        FLUID_ASSERT(collection != nullptr);
        FLUID_ASSERT(collection->is_type_present<MovementData3D>());

        auto& pos = collection->get<MovementData3D>(index);

        return calculate_grid_cell_location_of_position(pos.position);
    }

    HashedNeighborhoodSearch3D::GridCellLocation HashedNeighborhoodSearch3D::calculate_grid_cell_location_of_position(
            const glm::vec3& position) {
        return {(int)std::floor(position.x / search_radius), (int)std::floor(position.y / search_radius),
                (int)std::floor(position.z / search_radius)};
    }

    void HashedNeighborhoodSearch3D::find_neighbors() {
        improve_cache_efficiency();

        if (grid_rebuild_required) {
            rebuild_grid();
            grid_rebuild_required = false;
        } else {
            update_grid();
        }

        find_neighbors_with_grid();
    }

    void HashedNeighborhoodSearch3D::rebuild_grid() {
        FLUID_ASSERT(collection != nullptr);
        FLUID_ASSERT(collection->is_type_present<GridCellState>());
        FLUID_ASSERT(collection->is_type_present<ParticleInfo>());

        grid.clear();

        grid.auto_initialize_protection_enabled = false;
        for (particleIndex_t i = 0; i < collection->size(); i++) {
            if (collection->get<ParticleInfo>(i).type == ParticleTypeInactive) {
                continue;
            }

            auto& state = collection->get<GridCellState>(i);
            state.previous = GridCellLocation::undefined();
            state.current = calculate_grid_cell_location_of_particle(i);


            grid[state.current].insert(i);
        }
        grid.auto_initialize_protection_enabled = true;
    }

    void HashedNeighborhoodSearch3D::update_grid() {
        FLUID_ASSERT(collection != nullptr);
        FLUID_ASSERT(collection->is_type_present<GridCellState>());

        grid.auto_initialize_protection_enabled = false;
        for (particleIndex_t i = 0; i < collection->size(); i++) {
            if (collection->get<ParticleInfo>(i).type == ParticleTypeInactive) {
                // TODO: erase inactive particles from the grid
                continue;
            }


            auto& state = collection->get<GridCellState>(i);
            state.previous = state.current;
            state.current = calculate_grid_cell_location_of_particle(i);
            if (state.previous != state.current) {
                // particle needs to be moved or inserted
                if (state.previous == GridCellLocation::undefined()) {
                    grid[state.current].insert(i);
                } else {
                    grid[state.previous].erase(i);
                    grid[state.current].insert(i);
                }
            }
        }
        grid.auto_initialize_protection_enabled = true;
    }


    void HashedNeighborhoodSearch3D::find_neighbors_with_grid() {
        FLUID_ASSERT(collection != nullptr);
        FLUID_ASSERT(collection->is_type_present<MovementData3D>());
        FLUID_ASSERT(collection->is_type_present<ParticleInfo>());
        FLUID_ASSERT(collection->is_type_present<GridCellState>());
        FLUID_ASSERT(search_radius > 0.0f);

        if (collection->size() > neighbor_data.size())
            neighbor_data.resize(collection->size());

        float search_radius_squared = search_radius * search_radius;

        // search in parallel for each particle
        parallel::loop_for(0, collection->size(), [&](particleIndex_t i) {
            auto& data = neighbor_data[i];
            data.size = 0;

            if (collection->get<ParticleInfo>(i).type == ParticleTypeInactive)
                return;

            auto& mv_i = collection->get<MovementData3D>(i);
            auto& state = collection->get<GridCellState>(i);


            // iterate over the full 3x3 cube with the current cell in the center to cover all possible
            // cells that could contain neighbors of the current particle
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    for (int dz = -1; dz <= 1; dz++) {
                        // calculate the cell location of the current cube cell
                        GridCellLocation cell_to_check {state.current.x + dx, state.current.y + dy,
                                state.current.z + dz};

                        // iterate over the particles of the cube cell
                        for (particleIndex_t j : grid[cell_to_check]) {
                            auto& mv_j = collection->get<MovementData3D>(j);

                            glm::vec3 difference = mv_i.position - mv_j.position;

                            if (glm::dot(difference, difference) <= search_radius_squared) {
                                // the particle is a neighbor
                                if (data.neighbor_indices.size() <= data.size)
                                    data.neighbor_indices.resize(data.size + 1);
                                data.neighbor_indices[data.size] = j;
                                data.size++;
                            }
                        }
                    }
                }
            }
        });
    }

    void HashedNeighborhoodSearch3D::improve_cache_efficiency() {
        calls_since_last_cache_efficiency_improvement++;


        if (calls_since_last_cache_efficiency_improvement > 100) {
            ParticleCollectionAlgorithm::Sort sorter;
            sorter.adapt_collection(collection);

            sorter.quick_sort(
                    collection,
                    [&](const std::shared_ptr<ParticleCollection>& collection, const size_t index) -> uint64_t {
                        auto& grid_data = collection->get<GridCellState>(index);

                        auto transform_into_unsigned = [](int32_t v) -> uint32_t {
                            int64_t v2 = (int64_t)v;
                            v2 -= std::numeric_limits<int32_t>::min();
                            uint32_t res = (uint32_t)v2;
                            return res;
                        };


                        uint64_t key = libmorton::morton3D_64_encode(transform_into_unsigned(grid_data.current.x),
                                transform_into_unsigned(grid_data.current.y),
                                transform_into_unsigned(grid_data.current.z));
                        return key;
                    });

            calls_since_last_cache_efficiency_improvement = 0;
            grid_rebuild_required = true;
        }
    }


    HashedNeighborhoodSearch3D::Neighbors HashedNeighborhoodSearch3D::get_neighbors(particleIndex_t particleIndex) {
        Neighbors n;
        n.data = this;
        n.position_based = false;
        n.of.particle = particleIndex;
        return n;
    }

    HashedNeighborhoodSearch3D::Neighbors HashedNeighborhoodSearch3D::get_neighbors(const glm::vec3& position) {
        Neighbors n;
        n.data = this;
        n.position_based = true;
        n.of.position = position;
        return n;
    }


    bool HashedNeighborhoodSearch3D::NeighborsIterator::operator==(
            const LibFluid::HashedNeighborhoodSearch3D::NeighborsIterator& other) const {
        return data->data == other.data->data && current == other.current;
    }

    bool HashedNeighborhoodSearch3D::NeighborsIterator::operator!=(
            const HashedNeighborhoodSearch3D::NeighborsIterator& other) const {
        return !(*this == other);
    }

    HashedNeighborhoodSearch3D::particleIndex_t& HashedNeighborhoodSearch3D::NeighborsIterator::operator*() {
        FLUID_ASSERT(data != nullptr);
        FLUID_ASSERT(data->data != nullptr);
        if (!data->position_based) {
            FLUID_ASSERT(current >= 0);
            FLUID_ASSERT(data->data->collection->size() > current);

            FLUID_ASSERT(data->data->neighbor_data.size() > data->of.particle);
            FLUID_ASSERT(data->data->neighbor_data[data->of.particle].size > current);
            FLUID_ASSERT(data->data->neighbor_data[data->of.particle].neighbor_indices.size() > current);

            return data->data->neighbor_data[data->of.particle].neighbor_indices[current];
        } else {
            FLUID_ASSERT(data->data->collection != nullptr);
            FLUID_ASSERT(data->data->collection->size() > current);
            return current;
        }
    }

    const HashedNeighborhoodSearch3D::NeighborsIterator HashedNeighborhoodSearch3D::NeighborsIterator::operator++(int) {
        NeighborsIterator copy = *this;
        ++(*this);
        return copy;
    }

    HashedNeighborhoodSearch3D::NeighborsIterator& HashedNeighborhoodSearch3D::NeighborsIterator::operator++() {
        FLUID_ASSERT(data != nullptr);

        if (!data->position_based) {
            current++;
        } else {
            FLUID_ASSERT(data->data != nullptr);
            FLUID_ASSERT(data->data->collection != nullptr);
            FLUID_ASSERT(data->data->search_radius > 0.0f);
            auto collection = data->data->collection;
            bool found = false;

            auto center_cell = data->data->calculate_grid_cell_location_of_position(data->of.position);

            while (!found) {
                GridCellLocation current_cell {center_cell.x + dx, center_cell.y + dy, center_cell.z + dz};

                // initialize or advance the current iterator by one
                if (!current_set_iterator.has_value()) {
                    if (data->data->grid[current_cell].size() != 0) {
                        current_set_iterator = data->data->grid[current_cell].begin();
                    }
                } else {
                    (*current_set_iterator)++;
                    if (*current_set_iterator == data->data->grid[current_cell].end()) {
                        // set the iterator object to empty, since this iterator is depleted of entries
                        current_set_iterator.reset();
                    }
                }

                // check if we need to choose the next cell and therefore the next iterator
                if (!current_set_iterator.has_value()) {
                    // increment counters to the next cell location
                    dz++;
                    if (dz > 1) {
                        dy++;
                        dz = -1;
                    }
                    if (dy > 1) {
                        dx++;
                        dy = -1;
                    }
                    if (dx > 1) {
                        // there are no cells left to check, set the iterator to the end() and return
                        current = collection->size();
                        return *this;
                    }

                    // update the current cell
                    current_cell = {center_cell.x + dx, center_cell.y + dy, center_cell.z + dz};

                    // set the current iterator the the new cell
                    if (data->data->grid[current_cell].size() != 0) {
                        current_set_iterator = data->data->grid[current_cell].begin();
                    }

                    // repreat to check the next cell
                    found = false;
                    continue;
                } else {
                    // check the particle the iterator points to
                    particleIndex_t current_candidate = *(*current_set_iterator);
                    const glm::vec3& position = collection->get<MovementData3D>(current_candidate).position;
                    if (glm::length(data->of.position - position) <= data->data->search_radius) {
                        // we found a neighbor -> set the current particle index to the neighbor
                        current = current_candidate;
                        found = true;
                        continue;
                    } else {
                        // the candidate is not a neighbor, repeat to check the next particle
                        found = false;
                        continue;
                    }
                }
            }
        }
        return *this;
    }

    HashedNeighborhoodSearch3D::NeighborsIterator HashedNeighborhoodSearch3D::Neighbors::begin() const {
        FLUID_ASSERT(data != nullptr);
        NeighborsIterator iterator;
        iterator.data = this;
        iterator.current = -1;
        iterator++;
        return iterator;
    }

    HashedNeighborhoodSearch3D::NeighborsIterator HashedNeighborhoodSearch3D::Neighbors::end() const {
        FLUID_ASSERT(data != nullptr);
        NeighborsIterator iterator;
        iterator.data = this;
        if (!position_based) {
            FLUID_ASSERT(data->neighbor_data.size() > of.particle);
            iterator.current = data->neighbor_data[of.particle].size;
        } else {
            FLUID_ASSERT(data->collection != nullptr);
            iterator.current = data->collection->size();
        }
        return iterator;
    }


    std::shared_ptr<NeighborhoodInterface> HashedNeighborhoodSearch3D::create_interface() {
        auto res = std::make_shared<NeighborhoodInterface>();

        res->link.get_by_index = [this](particleIndex_t index) {
            auto neighbors = this->get_neighbors(index);

            auto n = NeighborhoodInterface::Neighbors();
            n.iterator_link.begin = [neighbors]() {
                auto real_it = neighbors.begin();
                return new NeighborsIterator(real_it);
            };
            n.iterator_link.end = [neighbors]() {
                auto real_it = neighbors.end();
                return new NeighborsIterator(real_it);
            };
            n.iterator_link.iterator_copy = [](void* it) {
                auto copy = new NeighborsIterator(*((NeighborsIterator*)it));
                return copy;
            };
            n.iterator_link.iterator_delete = [](void* it) {
                delete ((NeighborsIterator*)it);
            };
            n.iterator_link.iterator_dereference = [](void* it) {
                auto& index = *(*(NeighborsIterator*)it);
                return &index;
            };
            n.iterator_link.iterator_equals = [](void* it1, void* it2) {
                return *((NeighborsIterator*)it1) == *((NeighborsIterator*)it2);
            };
            n.iterator_link.iterator_increment = [](void* it) {
                ++(*(NeighborsIterator*)it);
            };

            return n;
        };

        res->link.get_by_position_3d = [this](const glm::vec3& position) {
            auto neighbors = this->get_neighbors(position);

            auto n = NeighborhoodInterface::Neighbors();
            n.iterator_link.begin = [neighbors]() {
                auto real_it = neighbors.begin();
                return new NeighborsIterator(real_it);
            };
            n.iterator_link.end = [neighbors]() {
                auto real_it = neighbors.end();
                return new NeighborsIterator(real_it);
            };
            n.iterator_link.iterator_copy = [](void* it) {
                auto copy = new NeighborsIterator(*((NeighborsIterator*)it));
                return copy;
            };
            n.iterator_link.iterator_delete = [](void* it) {
                delete ((NeighborsIterator*)it);
            };
            n.iterator_link.iterator_dereference = [](void* it) {
                auto& index = *(*(NeighborsIterator*)it);
                return &index;
            };
            n.iterator_link.iterator_equals = [](void* it1, void* it2) {
                return *((NeighborsIterator*)it1) == *((NeighborsIterator*)it2);
            };
            n.iterator_link.iterator_increment = [](void* it) {
                ++(*(NeighborsIterator*)it);
            };

            return n;
        };

        res->link.get_search_radius = [&] {
            return this->search_radius;
        };

        return res;
    }
} // namespace FluidSolver