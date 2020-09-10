#include "Simulation.hpp"
#include <algorithm>

void FluidSolver::Simulation::ExecuteSimulationStep() {
    if (fluidSolver == nullptr)
        return;
    if (particleCollection == nullptr)
        return;
    if (timestep == nullptr)
        return;

    // calculate timestep
    timestep->CalculateCurrentTimestep();
    float currentTimestep = timestep->getCurrentTimestep();
    // set timestep
   /* fluidSolver->setTimestep(currentTimestep);
    if (dataLogger != nullptr)
        dataLogger->setTimestep(currentTimestep);
    if (statisticCollector != nullptr)
        statisticCollector->setTimestep(currentTimestep);

    // apply external forces
    for (IExternalForce *externalForce : externalForces) {
        externalForce->Apply(particleCollection);
    }*/

    // simulate
    fluidSolver->execute_simulation_step(currentTimestep);

   /* // modify the simulation
    ISimulationModifier::SimulationInfo info;
    info.restDensity = restDensity;
    info.timeStep = currentTimestep;
    info.particleSize = particleSize;
    for (ISimulationModifier *modifier : simulationModifiers) {
        modifier->ModifySimulation(particleCollection, info);
    }*/

}

void FluidSolver::Simulation::VisualizeSimulation() {
    if (simulationVisualizer == nullptr)
        return;
    simulationVisualizer->Render();
}

void FluidSolver::Simulation::CollectStatistics() {
   /* // calculate the statistics
    if (statisticCollector != nullptr)
        statisticCollector->CalculateData();

    // log data
    if (dataLogger)
        dataLogger->TimeStepPassed();*/
}


FluidSolver::ParticleCollection *FluidSolver::Simulation::getParticleCollection() {
    return particleCollection;
}

void FluidSolver::Simulation::setParticleCollection(FluidSolver::ParticleCollection *particleCollection) {
    this->particleCollection = particleCollection;
    if (this->fluidSolver != nullptr)
        this->fluidSolver->collection = particleCollection;
    if (this->simulationVisualizer != nullptr)
        this->simulationVisualizer->setParticleCollection(particleCollection);

    if (this->timestep != nullptr)
        timestep->setParticleCollection(particleCollection);

}

FluidSolver::IISPHFluidSolver *FluidSolver::Simulation::getFluidSolver() {
    return fluidSolver;
}

void FluidSolver::Simulation::setFluidSolver(FluidSolver::IISPHFluidSolver *fluidSolver) {
    this->fluidSolver = fluidSolver;
    if (this->fluidSolver != nullptr) {
        fluidSolver->collection = this->particleCollection;
        fluidSolver->parameters.particle_size = this->particleSize;
        fluidSolver->parameters.rest_density = this->restDensity;
        fluidSolver->parameters.gravity = this->gravity;
        //fluidSolver->SetKernel(this->kernel);
        //fluidSolver->SetNeighborhoodSearch(this->neighborhoodSearch);
    }



}
/*
FluidSolver::StatisticCollector *FluidSolver::Simulation::getStatisticCollector() {
    return statisticCollector;
}

void FluidSolver::Simulation::setStatisticCollector(FluidSolver::StatisticCollector *statisticCollector) {
    this->statisticCollector = statisticCollector;
    if (dataLogger != nullptr)
        dataLogger->setStatisticCollector(statisticCollector);

    if (statisticCollector != nullptr) {
        statisticCollector->setParticleCollection(particleCollection);
        statisticCollector->setParticleSize(particleSize);
        statisticCollector->setRestDensity(restDensity);
        statisticCollector->setParticleSelection(particleSelection);
        statisticCollector->setGravity(gravity);
        statisticCollector->setFluidSolver(fluidSolver);
    }
}

DataLogger *FluidSolver::Simulation::getDataLogger() {
    return dataLogger;
}

void FluidSolver::Simulation::setDataLogger(DataLogger *dataLogger) {
    this->dataLogger = dataLogger;
    if (dataLogger != nullptr) {
        dataLogger->setStatisticCollector(statisticCollector);
        dataLogger->setFluidSolver(fluidSolver);
    }
}*/

FluidSolver::ISimulationVisualizer *FluidSolver::Simulation::getSimulationVisualizer() {
    return simulationVisualizer;
}

void FluidSolver::Simulation::setSimulationVisualizer(FluidSolver::ISimulationVisualizer *simulationVisualizer) {
    this->simulationVisualizer = simulationVisualizer;
    if (simulationVisualizer != nullptr) {
        simulationVisualizer->setParticleCollection(this->particleCollection);
        simulationVisualizer->setParticleSize(particleSize);
        simulationVisualizer->setRestDensity(restDensity);

    }
}

float FluidSolver::Simulation::getParticleSize() {
    return particleSize;
}

void FluidSolver::Simulation::setParticleSize(float particleSize) {
    this->particleSize = particleSize;
    if (fluidSolver != nullptr)
        fluidSolver->parameters.particle_size = particleSize;

    if (simulationVisualizer != nullptr)
        simulationVisualizer->setParticleSize(particleSize);
    if (timestep != nullptr)
        timestep->setParticleSize(particleSize);

}

float FluidSolver::Simulation::getRestDensity() {
    return restDensity;
}

void FluidSolver::Simulation::setRestDensity(float restDensity) {
    this->restDensity = restDensity;
    if (fluidSolver != nullptr)
        fluidSolver->parameters.rest_density = restDensity;
        if (simulationVisualizer != nullptr)
        simulationVisualizer->setRestDensity(restDensity);
}

FluidSolver::ITimestep *FluidSolver::Simulation::getTimestep() {
    return timestep;
}

void FluidSolver::Simulation::setTimestep(ITimestep *timestep) {
    this->timestep = timestep;
    if (timestep != nullptr) {
        timestep->setParticleCollection(particleCollection);
        timestep->setParticleSize(particleSize);
    }
}
/*
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
}*/

float FluidSolver::Simulation::getGravity() const {
    return gravity;
}

void FluidSolver::Simulation::setGravity(float gravity) {
    this->gravity = gravity;
    if (fluidSolver != nullptr)
        fluidSolver->parameters.gravity = gravity;
}
/*
void FluidSolver::Simulation::clearSimulationModifiers() {
    this->simulationModifiers.clear();
}

void FluidSolver::Simulation::clearExternalForces() {
    this->externalForces.clear();
}

FluidSolver::IParticleSelection *FluidSolver::Simulation::getParticleSelection() const {
    return particleSelection;
}

void FluidSolver::Simulation::setParticleSelection(FluidSolver::IParticleSelection *particleSelection) {
    this->particleSelection = particleSelection;
    if (statisticCollector != nullptr)
        statisticCollector->setParticleSelection(particleSelection);
    if (simulationVisualizer != nullptr)
        simulationVisualizer->setParticleSelection(particleSelection);
}

void FluidSolver::Simulation::setNeighborhoodSearch(FluidSolver::INeighborhoodSearch *neighborhoodSearch) {
    this->neighborhoodSearch = neighborhoodSearch;
    if (this->fluidSolver != nullptr) {
        this->fluidSolver->SetNeighborhoodSearch(this->neighborhoodSearch);
    }
}

FluidSolver::INeighborhoodSearch *FluidSolver::Simulation::getNeighborhoodSearch() {
    return this->neighborhoodSearch;
}

void FluidSolver::Simulation::setKernel(FluidSolver::IKernel *kernel) {
    this->kernel = kernel;
    if (this->fluidSolver != nullptr) {
        this->fluidSolver->SetKernel(this->kernel);
    }
}

FluidSolver::IKernel *FluidSolver::Simulation::getKernel() {
    return this->kernel;
}*/

float FluidSolver::Simulation::getRadius() {
    return 2.0f * particleSize;
}
