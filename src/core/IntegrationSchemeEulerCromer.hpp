//
// Created by corbi on 24.04.2019.
//

#ifndef FLUIDSOLVER_INTEGRATIONSCHEMEEULERCROMER_HPP
#define FLUIDSOLVER_INTEGRATIONSCHEMEEULERCROMER_HPP

#include "interface/IIntegrationScheme.hpp"

namespace FluidSolver {
    class IntegrationSchemeEulerCromer : public IIntegrationScheme {

    public:
        void Integrate(uint32_t particleIndex, IParticleCollection *particleCollection, float timeStep) override;
    };

}
#endif //FLUIDSOLVER_INTEGRATIONSCHEMEEULERCROMER_HPP
