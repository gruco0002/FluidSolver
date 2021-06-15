#ifndef FLUIDSOLVER_PARTICLEREMOVER_HPP
#define FLUIDSOLVER_PARTICLEREMOVER_HPP

#include "IEntity.hpp"

namespace FluidSolver
{
    class ParticleRemover : public IEntity {
      public:
        struct Parameters
        {
            Area area = {};
            bool remove_if_outside = true;
        } parameters;

        void initialize() override;

        void execute_simulation_step(pFloat timestep) override;
    };

} // namespace FluidSolver


#endif // FLUIDSOLVER_PARTICLEREMOVER_HPP
