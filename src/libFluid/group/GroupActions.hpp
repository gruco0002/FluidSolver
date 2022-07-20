#pragma once

#include "../FluidInclude.hpp"
#include "../fluidSolver/Particle.hpp"
#include "../fluidSolver/ParticleCollection.hpp"

namespace FluidSolver::GroupActions
{

    /**
     * Counts the number of particles inside the given group and returns it.
     * @tparam Group of particles.
     * A Group type has to provide the following things:
     *      GroupIterator
     *          An iterator that
     * allows to iterate over the particles in the group.
     *          Dereferencing the iterator yields a pIndex_t
     * particle index.
     *          The iterator must only support forward iterating and dereferencing to retrieve
     * the
     *          particle index.
     *      GroupIterator begin();
     *      GroupIterator end();
     *
     * bool is_member(pIndex_t index) const;
     *      ParticleCollection *collection = nullptr;
     * @param group A
     * group of particles
     * @return The amount of particles inside the group
     */
    template <typename Group> size_t count(const Group& group)
    {
        FLUID_ASSERT(group.collection != nullptr)
        size_t result = 0;
        for (pIndex_t index : group)
        {
            result++;
        }
        return result;
    }


    /**
     *
     * @tparam Group see description in FluidSolver::GroupActions::count
     * @param group
     *
     * @param type
     */
    template <typename Group> void set_type(const Group& group, ParticleType type)
    {
        FLUID_ASSERT(group.collection != nullptr)
        FLUID_ASSERT(group.collection->template is_type_present<ParticleInfo>())

        for (pIndex_t index : group)
        {
            group.collection->template get<ParticleInfo>(index).type = type;
        }
    }

    template <typename Group> void remove(const Group& group)
    {
        set_type<Group>(group, ParticleType::ParticleTypeDead);
    }


} // namespace FluidSolver::GroupActions
