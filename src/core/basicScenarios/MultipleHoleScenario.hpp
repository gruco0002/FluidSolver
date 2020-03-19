#ifndef FLUIDSOLVER_MULTIPLEHOLESCENARIO_HPP
#define FLUIDSOLVER_MULTIPLEHOLESCENARIO_HPP

#include "Scenario.hpp"
#include <core/fluidSolver/particleCollection/StripedParticleCollection.hpp>
#include <string>

namespace FluidSolver {
    class MultipleHoleScenario : public Scenario {

    public:
        void ResetData(IParticleCollection *particleCollection, float restDensity) override;

        int GetParticleCountX() override;

        int GetParticleCountY() override;

        std::vector<ISimulationModifier *> GetSimulationModifiers() override;

        float GetParticleSize() override;

        std::string GetName() override;

    private:

        const int factor = 4;

		void add(std::vector<FluidSolver::FluidParticle> *particles, float restDensity, float x, float y);
		void addBoundary(std::vector<FluidSolver::FluidParticle>* particles, float restDensity, float x, float y);
		void addDead(std::vector<FluidSolver::FluidParticle>* particles, float restDensity);

    };
}

#endif //FLUIDSOLVER_MULTIPLEHOLESCENARIO_HPP
