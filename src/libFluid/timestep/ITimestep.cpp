#include "ITimestep.hpp"
namespace FluidSolver {

    float ITimestep::get_current_timestep() const {
        return current_timestep;
    }

} // namespace FluidSolver
