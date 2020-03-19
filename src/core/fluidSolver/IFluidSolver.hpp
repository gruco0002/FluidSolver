#ifndef FLUIDSOLVER_IFLUIDSOLVER_HPP
#define FLUIDSOLVER_IFLUIDSOLVER_HPP

#include "core/fluidSolver/particleCollection/IParticleCollection.hpp"

namespace FluidSolver {

    class IFluidSolver {
    public:
        virtual void ExecuteSimulationStep() = 0;

        virtual float getParticleSize() = 0;

        virtual void setParticleSize(float particleSize) = 0;

        virtual float getRestDensity() = 0;

        virtual void setRestDensity(float restDensity) = 0;

        virtual float getTimestep() = 0;

        virtual void setTimestep(float timestep) = 0;

        virtual void setParticleCollection(IParticleCollection *particleCollection) = 0;

        virtual IParticleCollection *getParticleCollection() = 0;

        virtual float getGravity() = 0;

        virtual void setGravity(float gravity) = 0;

        virtual ~IFluidSolver() = default;

        virtual uint32_t GetComputationTimeLastTimestepInMicroseconds() = 0;

        virtual uint32_t GetComputationTimePressureSolverLastTimestepInMicroseconds() = 0;

    };


}
#endif //FLUIDSOLVER_IFLUIDSOLVER_HPP
