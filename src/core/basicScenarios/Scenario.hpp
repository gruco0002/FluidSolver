//
// Created by corbi on 28.05.2019.
//

#ifndef FLUIDSOLVER_SCENARIO_HPP
#define FLUIDSOLVER_SCENARIO_HPP

#include <core/fluidSolver/particleCollection/IParticleCollection.hpp>
#include <core/interface/ISimulationModifier.hpp>
#include <vector>

namespace FluidSolver {
    class Scenario {
    public:
        virtual void ResetData(IParticleCollection *particleCollection, float restDensity) = 0;

        virtual IParticleCollection *GenerateScenario(float restDensity);

        virtual float GetParticleSize();

        virtual bool ParticleCollectionTypeSupported(IParticleCollection *particleCollection);

        virtual int GetParticleCountX() = 0;

        virtual int GetParticleCountY() = 0;

        virtual std::vector<ISimulationModifier*> GetSimulationModifiers();
    };
}

#endif //FLUIDSOLVER_SCENARIO_HPP
