#include "ITimestepGenerator.hpp"
namespace FluidSolver {

    float ITimestepGenerator::get_generated_timestep() const {
        return generated_timestep;
    }

} // namespace FluidSolver
