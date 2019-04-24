//
// Created by corbi on 24.04.2019.
//

#ifndef FLUIDSOLVER_IINTEGRATIONSCHEME_HPP
#define FLUIDSOLVER_IINTEGRATIONSCHEME_HPP


#include <cstdint>
#include "IParticleCollection.hpp"

namespace FluidSolver {
    class IIntegrationScheme {

    public:

        virtual void Integrate(uint32_t particleIndex, IParticleCollection *particleCollection, float timeStep) = 0;

    };
}

#endif //FLUIDSOLVER_IINTEGRATIONSCHEME_HPP
