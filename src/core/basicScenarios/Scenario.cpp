
#include "core/basicScenarios/Scenario.hpp"


FluidSolver::ParticleCollection *FluidSolver::Scenario::GenerateScenario(float restDensity) {

    auto collection = CreateEmptyParticleCollection();

    this->ResetData(collection, restDensity);

    return collection;
}

float FluidSolver::Scenario::GetParticleSize() {
    return 1.0f;
}

/*std::vector<FluidSolver::ISimulationModifier *> FluidSolver::Scenario::GetSimulationModifiers() {
    return std::vector<FluidSolver::ISimulationModifier *>();
}*/

FluidSolver::ParticleCollection *FluidSolver::Scenario::CreateEmptyParticleCollection() {
    auto collection = new ParticleCollection();
    collection->add_type<MovementData>();
    collection->add_type<ParticleData>();
    collection->add_type<ParticleInfo>();
    collection->add_type<ExternalForces>();


    return collection;
}
