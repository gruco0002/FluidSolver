#include "ITimestepGenerator.hpp"
namespace FluidSolver {

    float ITimestepGenerator::get_current_timestep() const {
        return current_timestep;
    }

} // namespace FluidSolver
