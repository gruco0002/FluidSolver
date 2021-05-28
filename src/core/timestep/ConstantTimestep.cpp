#include "ConstantTimestep.hpp"

void FluidSolver::ConstantTimestep::calculate_current_timestep()
{
    current_timestep = settings.timestep;
}
