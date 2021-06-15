#ifndef FLUIDSOLVER_CONSTANTTIMESTEP_HPP
#define FLUIDSOLVER_CONSTANTTIMESTEP_HPP

#include "timestep/ITimestep.hpp"

namespace FluidSolver
{
    class ConstantTimestep : public ITimestep {
      public:
        struct ConstantTimestepSettings
        {
            float timestep = 0.001f;
        } settings;

        void calculate_current_timestep() override;

        Compatibility check() override;
    };
} // namespace FluidSolver

#endif // FLUIDSOLVER_CONSTANTTIMESTEP_HPP
