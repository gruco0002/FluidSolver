#ifndef FLUIDSOLVER_SCENARIO_HPP
#define FLUIDSOLVER_SCENARIO_HPP

#include <core/interface/ISimulationModifier.hpp>
#include <core/fluidSolver/ParticleCollection.hpp>
#include <vector>
#include <string>

namespace FluidSolver {
    class Scenario {
    protected:
        virtual ParticleCollection *CreateEmptyParticleCollection();

    public:
        virtual void ResetData(ParticleCollection *collection, float restDensity) = 0;

        virtual ParticleCollection *GenerateScenario(float restDensity);

        virtual float GetParticleSize();

        virtual int GetParticleCountX() = 0;

        virtual int GetParticleCountY() = 0;

        virtual std::vector<ISimulationModifier *> GetSimulationModifiers();

        virtual std::string GetName() = 0;
    };
}

#endif //FLUIDSOLVER_SCENARIO_HPP
