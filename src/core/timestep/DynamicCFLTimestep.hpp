#ifndef FLUIDSOLVER_DYNAMICCFLTIMESTEP_HPP
#define FLUIDSOLVER_DYNAMICCFLTIMESTEP_HPP

#include "ITimestep.hpp"

namespace FluidSolver
{
    class DynamicCFLTimestep : public ITimestep {
      public:
        struct DynamicCFLTimestepSettings
        {
            float max_timestep = 0.02f;
            float min_timestep = 0.0001f;
            float cfl_number = 0.8f;
        } settings;

        void calculate_current_timestep() override;

      private:
        float calculate_maximum_velocity();
    };
} // namespace FluidSolver


#endif // FLUIDSOLVER_DYNAMICCFLTIMESTEP_HPP
