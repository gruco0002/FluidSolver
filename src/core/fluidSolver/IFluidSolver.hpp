#ifndef FLUIDSOLVER_IFLUIDSOLVER_HPP
#define FLUIDSOLVER_IFLUIDSOLVER_HPP

#include <core/fluidSolver/kernel/IKernel.hpp>
#include <core/fluidSolver/neighborhoodSearch/INeighborhoodSearch.hpp>
#include <core/FluidSolverException.hpp>
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

        virtual void SetKernel(IKernel *kernel) = 0;

        virtual IKernel *GetKernel() = 0;

        virtual void SetNeighborhoodSearch(INeighborhoodSearch *neighborhoodSearch) = 0;

        virtual INeighborhoodSearch *GetNeighborhoodSearch() = 0;

        virtual inline void CheckSolverIntegrity() {
            // check solver for integrity
            if (this->GetNeighborhoodSearch() == nullptr)
                throw FluidSolverException("Neighborhood search is not set!");
            if (this->GetNeighborhoodSearch()->GetParticleCollection() != this->getParticleCollection())
                throw FluidSolverException(
                        "Neighborhood search particle collection is different from the particle collection of the solver!");
            if (this->GetKernel() == nullptr)
                throw FluidSolverException("Kernel is not set!");
            if (this->GetKernel()->GetKernelSupport() != this->GetNeighborhoodSearch()->GetRadius())
                throw FluidSolverException("Kernel support and neighborhood search radius are different!");
        }

    };


}
#endif //FLUIDSOLVER_IFLUIDSOLVER_HPP
