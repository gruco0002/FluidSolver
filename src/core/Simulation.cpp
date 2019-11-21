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
    if (simulationVisualizer == nullptr)
        return;
    simulationVisualizer->Render();
}

void FluidSolver::Simulation::CollectStatistics() {
    if (dataLogger)
        dataLogger->TimeStepPassed();
}


FluidSolver::IParticleCollection *FluidSolver::Simulation::getParticleCollection() {
    return particleCollection;
}

void FluidSolver::Simulation::setParticleCollection(FluidSolver::IParticleCollection *particleCollection) {
    Simulation::particleCollection = particleCollection;
    if (this->fluidSolver != nullptr)
        this->fluidSolver->setParticleCollection(particleCollection);
    if (this->simulationVisualizer != nullptr)
        this->simulationVisualizer->setParticleCollection(particleCollection);
}

FluidSolver::IFluidSolver *FluidSolver::Simulation::getFluidSolver() {
    return fluidSolver;
}

void FluidSolver::Simulation::setFluidSolver(FluidSolver::IFluidSolver *fluidSolver) {
    Simulation::fluidSolver = fluidSolver;
    fluidSolver->setParticleCollection(this->particleCollection);
    fluidSolver->setParticleSize(this->particleSize);
    fluidSolver->setRestDensity(this->restDensity);
    fluidSolver->setTimestep(this->timestep);
}

FluidSolver::StatisticCollector *FluidSolver::Simulation::getStatisticCollector() {
    return statisticCollector;
}

void FluidSolver::Simulation::setStatisticCollector(FluidSolver::StatisticCollector *statisticCollector) {
    Simulation::statisticCollector = statisticCollector;
}

DataLogger *FluidSolver::Simulation::getDataLogger() {
    return dataLogger;
}

void FluidSolver::Simulation::setDataLogger(DataLogger *dataLogger) {
    Simulation::dataLogger = dataLogger;
}

FluidSolver::ISimulationVisualizer *FluidSolver::Simulation::getSimulationVisualizer() {
    return simulationVisualizer;
}

void FluidSolver::Simulation::setSimulationVisualizer(FluidSolver::ISimulationVisualizer *simulationVisualizer) {
    Simulation::simulationVisualizer = simulationVisualizer;
    simulationVisualizer->setParticleCollection(this->particleCollection);
}

float FluidSolver::Simulation::getParticleSize() {
    return particleSize;
}

void FluidSolver::Simulation::setParticleSize(float particleSize) {
    Simulation::particleSize = particleSize;
    fluidSolver->setParticleSize(particleSize);
}

float FluidSolver::Simulation::getRestDensity() {
    return restDensity;
}

void FluidSolver::Simulation::setRestDensity(float restDensity) {
    Simulation::restDensity = restDensity;
    fluidSolver->setRestDensity(restDensity);
}

float FluidSolver::Simulation::getTimestep() {
    return timestep;
}

void FluidSolver::Simulation::setTimestep(float timestep) {
    Simulation::timestep = timestep;
    fluidSolver->setTimestep(timestep);
}
