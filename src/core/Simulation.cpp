//
// Created by corbi on 20.11.2019.
//

#include "Simulation.hpp"
#include <algorithm>

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
    this->particleCollection = particleCollection;
    if (this->fluidSolver != nullptr)
        this->fluidSolver->setParticleCollection(particleCollection);
    if (this->simulationVisualizer != nullptr)
        this->simulationVisualizer->setParticleCollection(particleCollection);
    if (statisticCollector != nullptr)
        statisticCollector->setParticleCollection(particleCollection);
}

FluidSolver::IFluidSolver *FluidSolver::Simulation::getFluidSolver() {
    return fluidSolver;
}

void FluidSolver::Simulation::setFluidSolver(FluidSolver::IFluidSolver *fluidSolver) {
    this->fluidSolver = fluidSolver;
    fluidSolver->setParticleCollection(this->particleCollection);
    fluidSolver->setParticleSize(this->particleSize);
    fluidSolver->setRestDensity(this->restDensity);
    fluidSolver->setTimestep(this->timestep);
    fluidSolver->setGravity(this->gravity);
}

FluidSolver::StatisticCollector *FluidSolver::Simulation::getStatisticCollector() {
    return statisticCollector;
}

void FluidSolver::Simulation::setStatisticCollector(FluidSolver::StatisticCollector *statisticCollector) {
    this->statisticCollector = statisticCollector;
    if (dataLogger != nullptr)
        dataLogger->setStatisticCollector(statisticCollector);

    statisticCollector->setTimestep(timestep);
    statisticCollector->setParticleCollection(particleCollection);
    statisticCollector->setParticleSize(particleSize);
    statisticCollector->setRestDensity(restDensity);
}

DataLogger *FluidSolver::Simulation::getDataLogger() {
    return dataLogger;
}

void FluidSolver::Simulation::setDataLogger(DataLogger *dataLogger) {
    this->dataLogger = dataLogger;
    dataLogger->setTimestep(timestep);
    dataLogger->setStatisticCollector(statisticCollector);
}

FluidSolver::ISimulationVisualizer *FluidSolver::Simulation::getSimulationVisualizer() {
    return simulationVisualizer;
}

void FluidSolver::Simulation::setSimulationVisualizer(FluidSolver::ISimulationVisualizer *simulationVisualizer) {
    this->simulationVisualizer = simulationVisualizer;
    simulationVisualizer->setParticleCollection(this->particleCollection);
    simulationVisualizer->setParticleSize(particleSize);
}

float FluidSolver::Simulation::getParticleSize() {
    return particleSize;
}

void FluidSolver::Simulation::setParticleSize(float particleSize) {
    this->particleSize = particleSize;
    if (fluidSolver != nullptr)
        fluidSolver->setParticleSize(particleSize);
    if (statisticCollector != nullptr)
        statisticCollector->setParticleSize(particleSize);
    if (simulationVisualizer != nullptr)
        simulationVisualizer->setParticleSize(particleSize);
}

float FluidSolver::Simulation::getRestDensity() {
    return restDensity;
}

void FluidSolver::Simulation::setRestDensity(float restDensity) {
    this->restDensity = restDensity;
    if (fluidSolver != nullptr)
        fluidSolver->setRestDensity(restDensity);
    if (statisticCollector != nullptr)
        statisticCollector->setRestDensity(restDensity);
}

float FluidSolver::Simulation::getTimestep() {
    return timestep;
}

void FluidSolver::Simulation::setTimestep(float timestep) {
    this->timestep = timestep;
    if (fluidSolver != nullptr)
        fluidSolver->setTimestep(timestep);
    if (dataLogger != nullptr)
        dataLogger->setTimestep(timestep);
    if (statisticCollector != nullptr)
        statisticCollector->setTimestep(timestep);
}

void FluidSolver::Simulation::addSimulationModifier(FluidSolver::ISimulationModifier *modifier) {
    simulationModifiers.push_back(modifier);
}

void FluidSolver::Simulation::removeSimulationModifier(FluidSolver::ISimulationModifier *modifier) {
    simulationModifiers.erase(std::remove(simulationModifiers.begin(), simulationModifiers.end(), modifier),
                              simulationModifiers.end());
}

void FluidSolver::Simulation::addExternalForce(FluidSolver::IExternalForce *force) {
    externalForces.push_back(force);
}

void FluidSolver::Simulation::removeExternalForce(FluidSolver::IExternalForce *force) {
    externalForces.erase(std::remove(externalForces.begin(), externalForces.end(), force), externalForces.end());
}

float FluidSolver::Simulation::getGravity() const {
    return gravity;
}

void FluidSolver::Simulation::setGravity(float gravity) {
    this->gravity = gravity;
    if (statisticCollector != nullptr)
        statisticCollector->setGravity(gravity);
    if (fluidSolver != nullptr)
        fluidSolver->setGravity(gravity);
}

void FluidSolver::Simulation::clearSimulationModifiers() {
    this->simulationModifiers.clear();
}

void FluidSolver::Simulation::clearExternalForces() {
    this->externalForces.clear();
}
