#ifndef FLUIDSOLVER_SIMPLEBOXSCENARIO_HPP
#define FLUIDSOLVER_SIMPLEBOXSCENARIO_HPP

#include <string>
#include "Scenario.hpp"

namespace FluidSolver {
    class SimpleBoxScenario : public Scenario {
    public:
        void ResetData(IParticleCollection *particleCollection, float restDensity) override;

        int GetParticleCountX() override;

        int GetParticleCountY() override;

        std::vector<ISimulationModifier *> GetSimulationModifiers() override;

        std::string GetName() override;
    };

}


#endif //FLUIDSOLVER_SIMPLEBOXSCENARIO_HPP
