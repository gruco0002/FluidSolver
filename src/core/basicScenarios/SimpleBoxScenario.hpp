//
// Created by corbi on 28.05.2019.
//

#ifndef FLUIDSOLVER_SIMPLEBOXSCENARIO_HPP
#define FLUIDSOLVER_SIMPLEBOXSCENARIO_HPP

#include "Scenario.hpp"

namespace FluidSolver {
    class SimpleBoxScenario : public Scenario {
    public:
        void ResetData(IParticleCollection *particleCollection, float restDensity) override;

        int GetParticleCountX() override;

        int GetParticleCountY() override;

        std::vector<ISimulationModifier *> GetSimulationModifiers() override;
    };

}


#endif //FLUIDSOLVER_SIMPLEBOXSCENARIO_HPP
