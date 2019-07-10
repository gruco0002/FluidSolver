//
// Created by corbi on 09.07.2019.
//

#ifndef FLUIDSOLVER_MULTIPLEHOLESCENARIO_HPP
#define FLUIDSOLVER_MULTIPLEHOLESCENARIO_HPP

#include "Scenario.hpp"
#include <core/SimpleParticleCollection.hpp>

namespace FluidSolver {
    class MultipleHoleScenario : public Scenario {

    public:
        void ResetData(IParticleCollection *particleCollection, float restDensity) override;

        int GetParticleCountX() override;

        int GetParticleCountY() override;

        std::vector<ISimulationModifier *> GetSimulationModifiers() override;

        float GetParticleSize() override;

    private:


		void add(std::vector<FluidSolver::SimpleParticleCollection::FluidParticle> *particles, float restDensity, float x, float y);
		void addBoundary(std::vector<FluidSolver::SimpleParticleCollection::FluidParticle>* particles, float restDensity, float x, float y);
		void addDead(std::vector<FluidSolver::SimpleParticleCollection::FluidParticle>* particles, float restDensity);

    };
}

#endif //FLUIDSOLVER_MULTIPLEHOLESCENARIO_HPP
