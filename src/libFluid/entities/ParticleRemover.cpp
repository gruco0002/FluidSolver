#include "ParticleRemover.hpp"

#include <group/GroupActions.hpp>
#include <group/RectangleAreaGroup.hpp>

void FluidSolver::ParticleRemover::initialize()
{
    return;
}

void FluidSolver::ParticleRemover::execute_simulation_step(FluidSolver::pFloat timestep)
{
    RectangleAreaGroup group;
    group.area = parameters.area;
    group.collection = sim.collection;
    group.inside = !parameters.remove_if_outside;
    GroupActions::remove(group);
}
