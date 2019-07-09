//
// Created by corbi on 09.07.2019.
//

#include "MultipleHoleScenario.hpp"

void
FluidSolver::MultipleHoleScenario::ResetData(FluidSolver::IParticleCollection *particleCollection, float restDensity) {


}

int FluidSolver::MultipleHoleScenario::GetParticleCountX() {
    return 0;
}

int FluidSolver::MultipleHoleScenario::GetParticleCountY() {
    return 0;
}

std::vector<FluidSolver::ISimulationModifier *> FluidSolver::MultipleHoleScenario::GetSimulationModifiers() {
    return Scenario::GetSimulationModifiers();
}
