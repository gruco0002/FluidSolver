#ifndef FLUIDSOLVER_IPARTICLESELECTION_HPP
#define FLUIDSOLVER_IPARTICLESELECTION_HPP

#include <cstdint>
#include <core/fluidSolver/particleCollection/IParticleCollection.hpp>

namespace FluidSolver {
    class IParticleSelection {
    public:
        virtual bool IsParticleSelected(uint32_t particleIndex, IParticleCollection *particleCollection) = 0;

        virtual ~IParticleSelection() = default;
    };
}

#endif //FLUIDSOLVER_IPARTICLESELECTION_HPP
