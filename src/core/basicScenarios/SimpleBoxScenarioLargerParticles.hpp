//
// Created by corbi on 11.12.2019.
//

#ifndef FLUIDSOLVER_SIMPLEBOXSCENARIOLARGERPARTICLES_HPP
#define FLUIDSOLVER_SIMPLEBOXSCENARIOLARGERPARTICLES_HPP


#include "Scenario.hpp"

namespace FluidSolver {
    class SimpleBoxScenarioLargerParticles : public Scenario {
    public:
        void ResetData(IParticleCollection *particleCollection, float restDensity) override;

        int GetParticleCountX() override;

        int GetParticleCountY() override;

        std::vector<ISimulationModifier *> GetSimulationModifiers() override;

        std::string GetName() override;

        float GetParticleSize() override;

        float Factor = 2.5f;
    };

}


#endif //FLUIDSOLVER_SIMPLEBOXSCENARIOLARGERPARTICLES_HPP
