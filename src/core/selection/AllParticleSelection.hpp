#ifndef FLUIDSOLVER_ALLPARTICLESELECTION_HPP
#define FLUIDSOLVER_ALLPARTICLESELECTION_HPP

#include "IParticleSelection.hpp"

namespace FluidSolver {
    class AllParticleSelection : public IParticleSelection{
    public:
        bool IsParticleSelected(uint32_t particleIndex, IParticleCollection *particleCollection) override;
    };
}


#endif //FLUIDSOLVER_ALLPARTICLESELECTION_HPP
