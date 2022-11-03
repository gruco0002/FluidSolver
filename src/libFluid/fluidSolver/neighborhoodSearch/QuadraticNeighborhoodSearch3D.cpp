#include "QuadraticNeighborhoodSearch3D.hpp"

#include "parallelization/StdParallelForEach.hpp"

namespace LibFluid {

    using parallel = StdParallelForEach;

    void QuadraticNeighborhoodSearch3D::initialize() {
    }

    void QuadraticNeighborhoodSearch3D::create_compatibility_report(CompatibilityReport& report) {
        report.begin_scope(FLUID_NAMEOF(QuadraticNeighborhoodSearch3D));
        if (collection == nullptr) {
            report.add_issue("ParticleCollection is null.");
        } else {
            if (!collection->is_type_present<MovementData3D>()) {
                report.add_issue("Particles are missing the MovementData3D attribute.");
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

    void QuadraticNeighborhoodSearch3D::find_neighbors() {
        FLUID_ASSERT(collection != nullptr);
        FLUID_ASSERT(collection->is_type_present<MovementData3D>());
        FLUID_ASSERT(collection->is_type_present<ParticleInfo>());
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
            for (particleIndex_t j = 0; j < collection->size(); j++) {
                if (collection->get<ParticleInfo>(j).type == ParticleTypeInactive)
                    continue;

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
        });
    }

    QuadraticNeighborhoodSearch3D::Neighbors QuadraticNeighborhoodSearch3D::get_neighbors(particleIndex_t particleIndex) {
        Neighbors n;
        n.data = this;
        n.position_based = false;
        n.of.particle = particleIndex;
        return n;
    }

    QuadraticNeighborhoodSearch3D::Neighbors QuadraticNeighborhoodSearch3D::get_neighbors(const glm::vec3& position) {
        Neighbors n;
        n.data = this;
        n.position_based = true;
        n.of.position = position;
        return n;
    }

    std::shared_ptr<NeighborhoodInterface> QuadraticNeighborhoodSearch3D::create_interface() {
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

        res->link.get_search_radius = [&]() {
            return this->search_radius;
        };

        return res;
    }

    bool QuadraticNeighborhoodSearch3D::NeighborsIterator::operator==(
            const LibFluid::QuadraticNeighborhoodSearch3D::NeighborsIterator& other) const {
        return data->data == other.data->data && current == other.current;
    }

    bool QuadraticNeighborhoodSearch3D::NeighborsIterator::operator!=(
            const QuadraticNeighborhoodSearch3D::NeighborsIterator& other) const {
        return !(*this == other);
    }

    QuadraticNeighborhoodSearch3D::particleIndex_t& QuadraticNeighborhoodSearch3D::NeighborsIterator::operator*() {
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

    const QuadraticNeighborhoodSearch3D::NeighborsIterator QuadraticNeighborhoodSearch3D::NeighborsIterator::operator++(
            int) {
        NeighborsIterator copy = *this;
        ++(*this);
        return copy;
    }

    QuadraticNeighborhoodSearch3D::NeighborsIterator& QuadraticNeighborhoodSearch3D::NeighborsIterator::operator++() {
        FLUID_ASSERT(data != nullptr);

        if (!data->position_based) {
            current++;
        } else {
            FLUID_ASSERT(data->data != nullptr);
            FLUID_ASSERT(data->data->collection != nullptr);
            FLUID_ASSERT(data->data->search_radius > 0.0f);
            auto collection = data->data->collection;
            current++;
            while (current < collection->size()) {
                const glm::vec3& position = collection->get<MovementData3D>(current).position;
                if (glm::length(data->of.position - position) <= data->data->search_radius) {
                    break;
                }
                current++;
            }
        }
        return *this;
    }

    QuadraticNeighborhoodSearch3D::NeighborsIterator QuadraticNeighborhoodSearch3D::Neighbors::begin() const {
        FLUID_ASSERT(data != nullptr);
        NeighborsIterator iterator;
        iterator.data = this;
        iterator.current = -1;
        iterator++;
        return iterator;
    }

    QuadraticNeighborhoodSearch3D::NeighborsIterator QuadraticNeighborhoodSearch3D::Neighbors::end() const {
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


} // namespace FluidSolver