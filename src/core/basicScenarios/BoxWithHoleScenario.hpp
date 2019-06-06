//
// Created by corbi on 06.06.2019.
//

#ifndef FLUIDSOLVER_BOXWITHHOLESCENARIO_HPP
#define FLUIDSOLVER_BOXWITHHOLESCENARIO_HPP

#include "Scenario.hpp"

namespace FluidSolver {
    class BoxWithHoleScenario : public Scenario {

    public:
        void ResetData(IParticleCollection *particleCollection, float restDensity) override;

        int GetParticleCountX() override;

        int GetParticleCountY() override;

    };
}

#endif //FLUIDSOLVER_BOXWITHHOLESCENARIO_HPP
