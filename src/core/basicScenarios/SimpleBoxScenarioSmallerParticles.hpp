//
// Created by corbi on 25.11.2019.
//

#ifndef FLUIDSOLVER_SIMPLEBOXSCENARIOSMALLERPARTICLES_HPP
#define FLUIDSOLVER_SIMPLEBOXSCENARIOSMALLERPARTICLES_HPP

#include "Scenario.hpp"

namespace FluidSolver {
    class SimpleBoxScenarioSmallerParticles : public Scenario {
    private:
        const int Factor = 2;
        const float Size =  1.0f/(float)Factor;
    public:
        float GetParticleSize() override;

        void ResetData(IParticleCollection *particleCollection, float restDensity) override;

        int GetParticleCountX() override;

        int GetParticleCountY() override;

        std::vector<ISimulationModifier *> GetSimulationModifiers() override;
    };
}


#endif //FLUIDSOLVER_SIMPLEBOXSCENARIOSMALLERPARTICLES_HPP
