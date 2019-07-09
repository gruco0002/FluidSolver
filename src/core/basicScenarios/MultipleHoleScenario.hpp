//
// Created by corbi on 09.07.2019.
//

#ifndef FLUIDSOLVER_MULTIPLEHOLESCENARIO_HPP
#define FLUIDSOLVER_MULTIPLEHOLESCENARIO_HPP

#include "Scenario.hpp"

namespace FluidSolver {
    class MultipleHoleScenario : public Scenario {

    public:
        void ResetData(IParticleCollection *particleCollection, float restDensity) override;

        int GetParticleCountX() override;

        int GetParticleCountY() override;

        std::vector<ISimulationModifier *> GetSimulationModifiers() override;

    };
}

#endif //FLUIDSOLVER_MULTIPLEHOLESCENARIO_HPP
