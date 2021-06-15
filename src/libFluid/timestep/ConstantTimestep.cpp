#include "ConstantTimestep.hpp"

void FluidSolver::ConstantTimestep::calculate_current_timestep()
{
    current_timestep = settings.timestep;
}

FluidSolver::Compatibility FluidSolver::ConstantTimestep::check()
{
    Compatibility c;
    if (settings.timestep <= 0.0f)
    {
        c.add_issue({"ConstantTimestep", "Timestep is smaller or equal to zero."});
    }
    return c;
}