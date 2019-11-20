//
// Created by corbi on 20.11.2019.
//

#include "Simulation.hpp"

void FluidSolver::Simulation::ExecuteSimulationStep() {
    if (fluidSolver == nullptr)
        return;
    if (particleCollection == nullptr)
        return;

    // apply external forces
    for (IExternalForce *externalForce : externalForces) {
        externalForce->Apply(particleCollection);
    }

    // simulate
    fluidSolver->ExecuteSimulationStep();


    // modify the simulation
    ISimulationModifier::SimulationInfo info;
    info.restDensity = restDensity;
    info.timeStep = timestep;
    info.particleSize = particleSize;
    for (ISimulationModifier *modifier : simulationModifiers) {
        modifier->ModifySimulation(particleCollection, info);
    }
}

void FluidSolver::Simulation::VisualizeSimulation() {
    if(simulationVisualizer == nullptr)
        return;
    simulationVisualizer->Render();
}

void FluidSolver::Simulation::CollectStatistics() {
    if (dataLogger)
        dataLogger->TimeStepPassed();
}
