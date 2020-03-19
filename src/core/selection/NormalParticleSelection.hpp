#ifndef FLUIDSOLVER_NORMALPARTICLESELECTION_HPP
#define FLUIDSOLVER_NORMALPARTICLESELECTION_HPP

#include "IParticleSelection.hpp"

namespace FluidSolver {
    class NormalParticleSelection : public IParticleSelection {
    public:
        bool IsParticleSelected(uint32_t particleIndex, IParticleCollection *particleCollection) override;
    };
}


#endif //FLUIDSOLVER_NORMALPARTICLESELECTION_HPP
