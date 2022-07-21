#include "ITimestepGenerator.hpp"
namespace LibFluid {

    float ITimestepGenerator::get_generated_timestep() const {
        return generated_timestep;
    }

} // namespace FluidSolver
