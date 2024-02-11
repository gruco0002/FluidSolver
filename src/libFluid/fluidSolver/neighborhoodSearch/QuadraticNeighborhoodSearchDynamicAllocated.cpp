#include "QuadraticNeighborhoodSearchDynamicAllocated.hpp"

#include "parallelization/StdParallelForEach.hpp"

#include <functional>

namespace LibFluid
{

    using parallel = StdParallelForEach;

    void QuadraticNeighborhoodSearchDynamicAllocated::find_neighbors()
    {
        FLUID_ASSERT(collection != nullptr);
        FLUID_ASSERT(search_radius > 0.0f);

        // create map entry for particles if not already existing
        for (particleIndex_t i = 0; i < collection->size(); i++)
        {
            // check if already allocated
            if (neighbors.find(i) == neighbors.end())
                neighbors[i] = std::vector<particleIndex_t>();
        }

        // find the neighbors by quadratic search

        parallel::loop_for(0, collection->size(), [&](particleIndex_t particleIndex) {
            // delete old neighbors of particles in the map
            neighbors[particleIndex].clear();

            // calculate neighbors for this particle
            const glm::vec2 &position = collection->get<MovementData>(particleIndex).position;
            for (particleIndex_t i = 0; i < collection->size(); i++)
            {
                glm::vec2 distVec = position - collection->get<MovementData>(i).position;
                if (glm::length(distVec) <= search_radius)
                {
                    // this is a neighbor: add it to the list inside the map
                    neighbors[particleIndex].push_back(i);
                }
            }
        });
    }

    QuadraticNeighborhoodSearchDynamicAllocated::Neighbors QuadraticNeighborhoodSearchDynamicAllocated::get_neighbors(
        particleIndex_t particleIndex)
    {
        Neighbors n;
        n.position_based = false;
        n.data = this;
        n.of.particle = particleIndex;
        return n;
    }

    QuadraticNeighborhoodSearchDynamicAllocated::Neighbors QuadraticNeighborhoodSearchDynamicAllocated::get_neighbors(
        const glm::vec2 &position)
    {
        Neighbors n;
        n.position_based = true;
        n.data = this;
        n.of.position = position;
        return n;
    }

    void QuadraticNeighborhoodSearchDynamicAllocated::initialize()
    {
    }

    void QuadraticNeighborhoodSearchDynamicAllocated::create_compatibility_report(CompatibilityReport &report)
    {
        report.begin_scope(FLUID_NAMEOF(QuadraticNeighborhoodSearchDynamicAllocated));
        if (collection == nullptr)
        {
            report.add_issue("ParticleCollection is null.");
        }
        else
        {
            if (!collection->is_type_present<MovementData>())
            {
                report.add_issue("Particles are missing the MovementData attribute.");
            }
            if (!collection->is_type_present<ParticleInfo>())
            {
                report.add_issue("Particles are missing the ParticleInfo attribute.");
            }
        }

        if (search_radius <= 0.0f)
        {
            report.add_issue("Search radius is smaller or equal to zero.");
        }

        report.end_scope();
    }

    std::shared_ptr<NeighborhoodInterface> QuadraticNeighborhoodSearchDynamicAllocated::create_interface()
    {
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
            n.iterator_link.iterator_copy = [](void *it) {
                auto copy = new NeighborsIterator(*((NeighborsIterator *)it));
                return copy;
            };
            n.iterator_link.iterator_delete = [](void *it) { delete ((NeighborsIterator *)it); };
            n.iterator_link.iterator_dereference = [](void *it) {
                auto &index = *(*(NeighborsIterator *)it);
                return &index;
            };
            n.iterator_link.iterator_equals = [](void *it1, void *it2) {
                return *((NeighborsIterator *)it1) == *((NeighborsIterator *)it2);
            };
            n.iterator_link.iterator_increment = [](void *it) { ++(*(NeighborsIterator *)it); };

            return n;
        };

        res->link.get_by_position = [this](const glm::vec2 &position) {
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
            n.iterator_link.iterator_copy = [](void *it) {
                auto copy = new NeighborsIterator(*((NeighborsIterator *)it));
                return copy;
            };
            n.iterator_link.iterator_delete = [](void *it) { delete ((NeighborsIterator *)it); };
            n.iterator_link.iterator_dereference = [](void *it) {
                auto &index = *(*(NeighborsIterator *)it);
                return &index;
            };
            n.iterator_link.iterator_equals = [](void *it1, void *it2) {
                return *((NeighborsIterator *)it1) == *((NeighborsIterator *)it2);
            };
            n.iterator_link.iterator_increment = [](void *it) { ++(*(NeighborsIterator *)it); };

            return n;
        };

        return res;
    }

    bool QuadraticNeighborhoodSearchDynamicAllocated::NeighborsIterator::operator==(
        const QuadraticNeighborhoodSearchDynamicAllocated::NeighborsIterator &other) const
    {
        return data->data == other.data->data && current == other.current;
    }

    bool QuadraticNeighborhoodSearchDynamicAllocated::NeighborsIterator::operator!=(
        const QuadraticNeighborhoodSearchDynamicAllocated::NeighborsIterator &other) const
    {
        return !(*this == other);
    }

    QuadraticNeighborhoodSearchDynamicAllocated::particleIndex_t &QuadraticNeighborhoodSearchDynamicAllocated::
        NeighborsIterator::operator*()
    {
        FLUID_ASSERT(data != nullptr);
        FLUID_ASSERT(data->data != nullptr);
        if (!data->position_based)
        {
            FLUID_ASSERT(data->data->neighbors.find(data->of.particle) != data->data->neighbors.end());
            FLUID_ASSERT(data->data->neighbors[data->of.particle].size() > current);
            FLUID_ASSERT(current >= 0);
            return data->data->neighbors[data->of.particle][current];
        }
        else
        {
            FLUID_ASSERT(data->data->collection != nullptr);
            FLUID_ASSERT(data->data->collection->size() > current);
            return current;
        }
    }

    const QuadraticNeighborhoodSearchDynamicAllocated::NeighborsIterator QuadraticNeighborhoodSearchDynamicAllocated::
        NeighborsIterator::operator++(int)
    {
        FLUID_ASSERT(data != nullptr);

        NeighborsIterator copy = *this;
        if (!data->position_based)
        {
            current++;
        }
        else
        {
            FLUID_ASSERT(data->data != nullptr);
            FLUID_ASSERT(data->data->collection != nullptr);
            FLUID_ASSERT(data->data->search_radius > 0.0f);
            auto collection = data->data->collection;
            current++;
            while (current < collection->size())
            {
                const glm::vec2 &position = collection->get<MovementData>(current).position;
                if (glm::length(data->of.position - position) <= data->data->search_radius)
                {
                    break;
                }
                current++;
            }
        }
        return copy;
    }

    QuadraticNeighborhoodSearchDynamicAllocated::NeighborsIterator &QuadraticNeighborhoodSearchDynamicAllocated::
        NeighborsIterator::operator++()
    {
        FLUID_ASSERT(data != nullptr);

        if (!data->position_based)
        {
            current++;
        }
        else
        {
            FLUID_ASSERT(data->data != nullptr);
            FLUID_ASSERT(data->data->collection != nullptr);
            FLUID_ASSERT(data->data->search_radius > 0.0f);
            auto collection = data->data->collection;
            current++;
            while (current < collection->size())
            {
                const glm::vec2 &position = collection->get<MovementData>(current).position;
                if (glm::length(data->of.position - position) <= data->data->search_radius)
                {
                    break;
                }
                current++;
            }
        }
        return *this;
    }

    QuadraticNeighborhoodSearchDynamicAllocated::NeighborsIterator QuadraticNeighborhoodSearchDynamicAllocated::
        Neighbors::begin() const
    {
        FLUID_ASSERT(data != nullptr);
        NeighborsIterator iterator;
        iterator.data = this;
        iterator.current = -1;
        iterator++;
        return iterator;
    }

    QuadraticNeighborhoodSearchDynamicAllocated::NeighborsIterator QuadraticNeighborhoodSearchDynamicAllocated::
        Neighbors::end() const
    {
        FLUID_ASSERT(data != nullptr);
        NeighborsIterator iterator;
        iterator.data = this;
        if (!position_based)
        {
            FLUID_ASSERT(data->neighbors.find(of.particle) != data->neighbors.end());
            iterator.current = data->neighbors[of.particle].size();
        }
        else
        {
            FLUID_ASSERT(data->collection != nullptr);
            iterator.current = data->collection->size();
        }
        return iterator;
    }
} // namespace LibFluid
