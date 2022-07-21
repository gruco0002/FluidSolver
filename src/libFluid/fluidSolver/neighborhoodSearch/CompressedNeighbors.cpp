#include "CompressedNeighbors.hpp"

#include "fluidSolver/ParticleCollectionAlgorithm.hpp"
#include "parallelization/StdParallelForEach.hpp"

#include <array>
#include <cstring>
#include <libmorton/morton.h>

namespace LibFluid {

    using parallel = StdParallelForEach;


    void CompressedNeighborhoodSearch::create_compatibility_report(CompatibilityReport& report) {
        report.begin_scope(FLUID_NAMEOF(CompressedNeighborhoodSearch));
        if (collection == nullptr) {
            report.add_issue("ParticleCollection is null.");
        } else {
            if (!collection->is_type_present<MovementData3D>()) {
                report.add_issue(
                        "Particles are missing the MovementData3D attribute.");
            }
            if (!collection->is_type_present<ParticleInfo>()) {
                report.add_issue("Particles are missing the ParticleInfo attribute.");
            }
        }

        if (search_radius <= 0.0f) {
            report.add_issue("Search radius is smaller or equal to zero.");
        }

        report.end_scope();
    }

    CompressedNeighborhoodSearch::GridCellLocation CompressedNeighborhoodSearch::
            calculate_grid_cell_location_of_position(const vec3& position) {
        return {(int)std::floor(position.x / search_radius), (int)std::floor(position.y / search_radius),
                (int)std::floor(position.z / search_radius)};
    }

    uint64_t CompressedNeighborhoodSearch::calculate_cell_index_by_cell_location(const GridCellLocation& location) {
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

    size_t CompressedNeighborhoodSearch::get_particle_index_by_cell_index(size_t cell_index) const {
        std::function<size_t(size_t, size_t)> recursive_search = [&](size_t left, size_t right) -> size_t {
            if (left >= cell_to_particle_map.size() || right == (size_t)(-1) || left == right) {
                // could not find entry
                return -1;
            }

            size_t middle = (right - left) / 2 + left;
            FLUID_ASSERT(middle < cell_to_particle_map.size());

            if (cell_to_particle_map[middle].cell_index > cell_index) {
                // search in the left part
                return recursive_search(left, middle);
            } else if (cell_to_particle_map[middle].cell_index < cell_index) {
                // search in the right part
                return recursive_search(middle + 1, right);
            } else {
                // we found the cell index
                return cell_to_particle_map[middle].index_of_first_particle;
            }
        };

        return recursive_search(0, cell_to_particle_map.size());
    }

    void CompressedNeighborhoodSearch::initialize() {
        if (!collection->is_type_present<ParticleInformation>()) {
            collection->add_type<ParticleInformation>();
        }
        if (!collection->is_type_present<NeighborStorage>()) {
            collection->add_type<NeighborStorage>();
        }
    }

    std::shared_ptr<NeighborhoodInterface> CompressedNeighborhoodSearch::create_interface() {
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

        res->link.get_by_position_3d = [this](const vec3& position) {
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

    void CompressedNeighborhoodSearch::find_neighbors() {
        FLUID_ASSERT(collection != nullptr);
        FLUID_ASSERT(collection->is_type_present<ParticleInformation>());
        FLUID_ASSERT(collection->is_type_present<NeighborStorage>());

        // calculate grid cell and cell index of each particle
        parallel::loop_for(0, collection->size(), [&](size_t particle_index) {
            const auto& particle_info = collection->get<ParticleInfo>(particle_index);
            auto& information = collection->get<ParticleInformation>(particle_index);
            if (particle_info.type == ParticleTypeDead) {
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

            sorter.quick_sort_stable<false, true>(
                    collection,
                    [](const std::shared_ptr<ParticleCollection>& collection, const pIndex_t index) -> uint64_t {
                        auto& information = collection->get<ParticleInformation>(index);
                        return information.cell_index;
                    });
        }

        // build map that maps cell to cell start
        {
            cell_to_particle_map.clear();
            for (size_t particle_index = 0; particle_index < collection->size(); particle_index++) {
                const auto& particle_info = collection->get<ParticleInfo>(particle_index);
                if (particle_info.type == ParticleTypeDead) {
                    break;
                }

                auto& information = collection->get<ParticleInformation>(particle_index);
                if (particle_index == 0) {
                    information.first_particle_of_cell = true;
                    cell_to_particle_map.push_back({information.cell_location, particle_index, information.cell_index});
                    continue;
                }

                const auto& prev_information = collection->get<ParticleInformation>(particle_index - 1);
                if (prev_information.cell_index != information.cell_index) {
                    information.first_particle_of_cell = true;
                    cell_to_particle_map.push_back({information.cell_location, particle_index, information.cell_index});
                }
            }
        }

        // find neighbors of each particle
        {
            parallel::loop_for(0, collection->size(), [&](size_t particle_index) {
                const auto& pi = collection->get<ParticleInfo>(particle_index);
                if (pi.type == ParticleTypeDead) {
                    return;
                }
                const auto& mv_particle = collection->get<MovementData3D>(particle_index);
                auto& storage = collection->get<NeighborStorage>(particle_index);

                find_neighbors_and_save_in_storage(mv_particle.position, storage);
            });
        }
    }

    CompressedNeighborhoodSearch::Neighbors CompressedNeighborhoodSearch::get_neighbors(particleIndex_t particleIndex) {
        Neighbors ret;
        ret.of.particle = particleIndex;
        ret.position_based = false;
        ret.data = this;
        return ret;
    }

    CompressedNeighborhoodSearch::Neighbors CompressedNeighborhoodSearch::get_neighbors(const vec3& position) {
        Neighbors ret;
        ret.of.position = position;
        ret.position_based = true;
        ret.data = this;
        ret.calculate_position_based_neighbors();
        return ret;
    }


    void CompressedNeighborhoodSearch::NeighborStorage::clear() {
        control_sequence.reset();
        current_deltas_byte_size = 0;
        size_value = 0;
        first_neighbor = -1;
        for (size_t i = 0; i < DELTAS_SIZE; i++) {
            deltas[i] = 0;
        }
    }

    void CompressedNeighborhoodSearch::NeighborStorage::set_first_neighbor(size_t index) {
        first_neighbor = index;
        size_value = 1;
        current_deltas_byte_size = 0;
    }

    void CompressedNeighborhoodSearch::NeighborStorage::set_next_neighbor(size_t delta_to_previous_neighbor) {
        FLUID_ASSERT(size_value - 1 < MAX_DELTAS);
        FLUID_ASSERT(delta_to_previous_neighbor != 0);

        size_t bit_pair_index = size_value - 1;
        if (delta_to_previous_neighbor == 1) {
            // zero byte representation
            control_sequence.set(bit_pair_index * 2 + 0, false);
            control_sequence.set(bit_pair_index * 2 + 1, false);
        } else if (delta_to_previous_neighbor == 2) {
            // zero byte representation
            control_sequence.set(bit_pair_index * 2 + 0, false);
            control_sequence.set(bit_pair_index * 2 + 1, true);
        } else if (delta_to_previous_neighbor <= 256 + 2) {
            FLUID_ASSERT(current_deltas_byte_size < DELTAS_SIZE);

            // one byte representation of the delta
            control_sequence.set(bit_pair_index * 2 + 0, true);
            control_sequence.set(bit_pair_index * 2 + 1, false);

            // save a 8 bit unsigned integer inside the delta array
            // note that we subtract the already covered range to allow for larger delta values this 8 bits can
            // represent
            deltas[current_deltas_byte_size] = (uint8_t)(delta_to_previous_neighbor - 3);
            current_deltas_byte_size++;
        } else {
            FLUID_ASSERT(current_deltas_byte_size + 3 < DELTAS_SIZE);

            // four byte representation of the delta
            control_sequence.set(bit_pair_index * 2 + 0, true);
            control_sequence.set(bit_pair_index * 2 + 1, true);

            // save a 32 bit unsigned integer inside the delta array
            // note that we subtract the already covered range to allow for larger delta values this 32 bits can
            // represent
            uint32_t reduced_delta = delta_to_previous_neighbor - (256 + 3);
            deltas[current_deltas_byte_size + 0] = reduced_delta & 0xff;
            deltas[current_deltas_byte_size + 1] = (reduced_delta >> 8) & 0xff;
            deltas[current_deltas_byte_size + 2] = (reduced_delta >> 16) & 0xff;
            deltas[current_deltas_byte_size + 3] = (reduced_delta >> 24) & 0xff;

            current_deltas_byte_size += 4;
        }

        size_value++;
    }

    size_t CompressedNeighborhoodSearch::NeighborStorage::size() const {
        return size_value;
    }

    uint32_t CompressedNeighborhoodSearch::NeighborStorage::get_first_neighbor() const {
        return first_neighbor;
    }

    size_t CompressedNeighborhoodSearch::NeighborStorage::get_used_delta_bytes() const {
        return current_deltas_byte_size;
    }

    uint32_t CompressedNeighborhoodSearch::NeighborStorage::get_delta(size_t delta_index) const {
        FLUID_ASSERT(delta_index < MAX_DELTAS);
        if (control_sequence[delta_index * 2 + 0] == false) {
            if (control_sequence[delta_index * 2 + 1] == false) {
                return 1;
            } else {
                return 2;
            }
        } else {
            size_t starting_byte_index = 0;
            // we have to determine the byte position
            for (size_t i = 0; i < delta_index; i++) {
                if (control_sequence[i * 2 + 0] == false) {
                    // this sequence does not need any bytes to store its information
                    continue;
                }

                if (control_sequence[i * 2 + 1] == false) {
                    // this sequence takes one byte to store its information
                    starting_byte_index += 1;
                } else {
                    // this sequence takes four bytes to store its information
                    starting_byte_index += 4;
                }
            }


            if (control_sequence[delta_index * 2 + 1] == false) {
                // the delta value is represented by one byte
                FLUID_ASSERT(starting_byte_index < DELTAS_SIZE);
                return deltas[starting_byte_index] + 3;
            } else {
                // the delta value is represented by four bytes
                FLUID_ASSERT(starting_byte_index + 3 < DELTAS_SIZE);

                uint32_t value = uint32_t((uint8_t)(deltas[starting_byte_index + 3]) << 24 |
                        (uint8_t)(deltas[starting_byte_index + 2]) << 16 |
                        (uint8_t)(deltas[starting_byte_index + 1]) << 8 |
                        (uint8_t)(deltas[starting_byte_index + 0]));
                value = value + 3 + 256;
                FLUID_ASSERT(value != std::numeric_limits<uint32_t>::max());
                return value;
            }
        }
    }

    CompressedNeighborhoodSearch::NeighborsIterator CompressedNeighborhoodSearch::Neighbors::begin() const {
        FLUID_ASSERT(data != nullptr);
        FLUID_ASSERT(data->collection != nullptr);

        NeighborsIterator iterator;
        iterator.data = this;
        iterator.current_counter = 0;

        if (position_based) {
            if (internal_storage.size() != 0) {
                iterator.current = internal_storage.get_first_neighbor();
            } else {
                // the iterator reached already the end
                iterator.current_counter = -1;
                iterator.current = data->collection->size();
            }
        } else {
            FLUID_ASSERT(of.particle < data->collection->size());

            const auto& storage = data->collection->get<NeighborStorage>(of.particle);
            if (storage.size() != 0) {
                iterator.current = storage.get_first_neighbor();
            } else {
                // the iterator reached already the end
                iterator.current_counter = -1;
                iterator.current = data->collection->size();
            }
        }

        return iterator;
    }

    CompressedNeighborhoodSearch::NeighborsIterator CompressedNeighborhoodSearch::Neighbors::end() const {
        FLUID_ASSERT(data != nullptr);
        FLUID_ASSERT(data->collection != nullptr);

        NeighborsIterator iterator;
        iterator.data = this;
        iterator.current = data->collection->size();
        iterator.current_counter = -1;
        return iterator;
    }


    bool CompressedNeighborhoodSearch::NeighborsIterator::operator==(
            const LibFluid::CompressedNeighborhoodSearch::NeighborsIterator& other) const {
        return data->data == other.data->data && current == other.current && current_counter == other.current_counter;
    }

    bool CompressedNeighborhoodSearch::NeighborsIterator::operator!=(
            const CompressedNeighborhoodSearch::NeighborsIterator& other) const {
        return !(*this == other);
    }

    CompressedNeighborhoodSearch::particleIndex_t& CompressedNeighborhoodSearch::NeighborsIterator::operator*() {
        FLUID_ASSERT(data != nullptr);
        FLUID_ASSERT(data->data != nullptr);
        FLUID_ASSERT(data->data->collection != nullptr);
        FLUID_ASSERT(data->data->collection->size() > current);
        return current;
    }

    const CompressedNeighborhoodSearch::NeighborsIterator CompressedNeighborhoodSearch::NeighborsIterator::operator++(
            int) {
        NeighborsIterator copy = *this;
        ++(*this);
        return copy;
    }

    CompressedNeighborhoodSearch::NeighborsIterator& CompressedNeighborhoodSearch::NeighborsIterator::operator++() {
        FLUID_ASSERT(data != nullptr);
        FLUID_ASSERT(data->data != nullptr);
        CompressedNeighborhoodSearch* compressed = data->data;
        FLUID_ASSERT(compressed->collection != nullptr);


        if (!data->position_based) {
            FLUID_ASSERT(data->of.particle < compressed->collection->size());
            const auto& storage = compressed->collection->get<NeighborStorage>(data->of.particle);
            current_counter++;
            if (current_counter < storage.size()) {
                size_t delta = storage.get_delta(current_counter - 1);
                current += delta;

                FLUID_ASSERT(current < compressed->collection->size());
            } else {
                // we reached the end
                current_counter = -1;
                current = compressed->collection->size();
            }
        } else {
            const auto& storage = data->internal_storage;
            current_counter++;
            if (current_counter < storage.size()) {
                size_t delta = storage.get_delta(current_counter - 1);
                current += delta;

                FLUID_ASSERT(current < compressed->collection->size());
            } else {
                // we reached the end
                current_counter = -1;
                current = compressed->collection->size();
            }
        }
        return *this;
    }

    CompressedNeighborhoodSearch::NeighborStorage::NeighborStorage() {
        clear();
    }

    CompressedNeighborhoodSearch::NeighborStorage::NeighborStorage(const NeighborStorage& c) {
        first_neighbor = c.first_neighbor;
        control_sequence = c.control_sequence;
        size_value = c.size_value;
        current_deltas_byte_size = c.current_deltas_byte_size;
        std::memcpy(deltas, c.deltas, DELTAS_SIZE * sizeof(uint8_t));
    }

    CompressedNeighborhoodSearch::NeighborStorage::NeighborStorage(NeighborStorage&& c) {
        first_neighbor = c.first_neighbor;
        control_sequence = c.control_sequence;
        size_value = c.size_value;
        current_deltas_byte_size = c.current_deltas_byte_size;
        std::memcpy(deltas, c.deltas, DELTAS_SIZE * sizeof(uint8_t));
    }

    CompressedNeighborhoodSearch::NeighborStorage& CompressedNeighborhoodSearch::NeighborStorage::operator=(
            NeighborStorage&& c) {
        first_neighbor = c.first_neighbor;
        control_sequence = c.control_sequence;
        size_value = c.size_value;
        current_deltas_byte_size = c.current_deltas_byte_size;
        std::memcpy(deltas, c.deltas, DELTAS_SIZE * sizeof(uint8_t));
        return *this;
    }

    CompressedNeighborhoodSearch::NeighborStorage& CompressedNeighborhoodSearch::NeighborStorage::operator=(
            const NeighborStorage& c) {
        first_neighbor = c.first_neighbor;
        control_sequence = c.control_sequence;
        size_value = c.size_value;
        current_deltas_byte_size = c.current_deltas_byte_size;
        std::memcpy(deltas, c.deltas, DELTAS_SIZE * sizeof(uint8_t));
        return *this;
    }


    void CompressedNeighborhoodSearch::Neighbors::calculate_position_based_neighbors() {
        FLUID_ASSERT(data != nullptr);
        FLUID_ASSERT(position_based == true);

        data->find_neighbors_and_save_in_storage(of.position, internal_storage);
    }

    void CompressedNeighborhoodSearch::find_neighbors_and_save_in_storage(const vec3& position,
            NeighborStorage& storage) {
        FLUID_ASSERT(collection != nullptr);

        auto cell_location = calculate_grid_cell_location_of_position(position);

        // determine the cells that need to be checked
        std::array<size_t, 27> cell_indices_to_check;
        size_t i = 0;
        for (int x = -1; x <= 1; x++) {
            for (int y = -1; y <= 1; y++) {
                for (int z = -1; z <= 1; z++) {
                    FLUID_ASSERT(i < cell_indices_to_check.size());

                    cell_indices_to_check[i] = calculate_cell_index_by_cell_location({
                            cell_location.x + x,
                            cell_location.y + y,
                            cell_location.z + z,
                    });
                    i++;
                }
            }
        }
        std::sort(cell_indices_to_check.begin(), cell_indices_to_check.end());

        storage.clear();
        size_t last_neighbor = -1;
        size_t delta_counter = 0;

        for (size_t i = 0; i < cell_indices_to_check.size(); i++) {
            size_t first_particle_index = get_particle_index_by_cell_index(cell_indices_to_check[i]);
            if (first_particle_index == (size_t)(-1)) {
                // the cell is empty and therefore non existant
                continue;
            }

            size_t current_particle = first_particle_index;
            while (cell_indices_to_check[i] == collection->get<ParticleInformation>(current_particle).cell_index) {
                const auto& mv_current = collection->get<MovementData3D>(current_particle);
                auto diff = mv_current.position - position;
                if (glm::dot(diff, diff) <= Math::pow2(search_radius)) {
                    // the particles are neighbors
                    if (last_neighbor == (size_t)(-1)) {
                        last_neighbor = current_particle;
                        storage.set_first_neighbor(last_neighbor);
                    } else {
                        FLUID_ASSERT(last_neighbor < current_particle);
                        size_t delta = current_particle - last_neighbor;
                        storage.set_next_neighbor(delta);
                        last_neighbor = current_particle;
                        delta_counter++;
                    }
                }
                current_particle++;
                if (current_particle >= collection->size()) {
                    break;
                }
            }
        }
    }

} // namespace FluidSolver
