//
// Created by corbi on 28.05.2019.
//

#ifndef FLUIDSOLVER_BOUNDARYTESTSCENARIO_HPP
#define FLUIDSOLVER_BOUNDARYTESTSCENARIO_HPP

#include "Scenario.hpp"

namespace FluidSolver {

    class BoundaryTestScenario : public Scenario {
    public:
        void ResetData(IParticleCollection *particleCollection, float restDensity) override;

        int GetParticleCountX() override;

        int GetParticleCountY() override;
    };

}

#endif //FLUIDSOLVER_BOUNDARYTESTSCENARIO_HPP
