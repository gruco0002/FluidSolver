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
    fluidSolver->setTimestep(currentTimestep);
    if (dataLogger != nullptr)
        dataLogger->setTimestep(currentTimestep);
    if (statisticCollector != nullptr)
        statisticCollector->setTimestep(currentTimestep);

    // apply external forces
    for (IExternalForce *externalForce : externalForces) {
        externalForce->Apply(particleCollection);
    }

    // simulate
    fluidSolver->ExecuteSimulationStep();

    // modify the simulation
    ISimulationModifier::SimulationInfo info;
    info.restDensity = restDensity;
    info.timeStep = currentTimestep;
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
    // calculate the statistics
    if (statisticCollector != nullptr)
        statisticCollector->CalculateData();

    // log data
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
    if (this->timestep != nullptr)
        timestep->setParticleCollection(particleCollection);
    if (neighborhoodSearch != nullptr) {
        auto newSearch = this->neighborhoodSearch->CreateCopy(this->getParticleCollection(), getRadius());
        auto oldSearch = this->getNeighborhoodSearch();
        this->setNeighborhoodSearch(newSearch);
        delete oldSearch;
    }
}

FluidSolver::IFluidSolver *FluidSolver::Simulation::getFluidSolver() {
    return fluidSolver;
}

void FluidSolver::Simulation::setFluidSolver(FluidSolver::IFluidSolver *fluidSolver) {
    this->fluidSolver = fluidSolver;
    if (this->fluidSolver != nullptr) {
        fluidSolver->setParticleCollection(this->particleCollection);
        fluidSolver->setParticleSize(this->particleSize);
        fluidSolver->setRestDensity(this->restDensity);
        fluidSolver->setGravity(this->gravity);
        fluidSolver->SetKernel(this->kernel);
        fluidSolver->SetNeighborhoodSearch(this->neighborhoodSearch);
    }

    if (dataLogger != nullptr)
        dataLogger->setFluidSolver(fluidSolver);
    if (statisticCollector != nullptr)
        statisticCollector->setFluidSolver(fluidSolver);
}

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
}

FluidSolver::ISimulationVisualizer *FluidSolver::Simulation::getSimulationVisualizer() {
    return simulationVisualizer;
}

void FluidSolver::Simulation::setSimulationVisualizer(FluidSolver::ISimulationVisualizer *simulationVisualizer) {
    this->simulationVisualizer = simulationVisualizer;
    if (simulationVisualizer != nullptr) {
        simulationVisualizer->setParticleCollection(this->particleCollection);
        simulationVisualizer->setParticleSize(particleSize);
        simulationVisualizer->setRestDensity(restDensity);
        simulationVisualizer->setParticleSelection(particleSelection);
    }
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
    if (timestep != nullptr)
        timestep->setParticleSize(particleSize);
    if (kernel != nullptr) {
        auto newKernel = this->kernel->CreateCopy(getRadius());
        auto oldKernel = this->getKernel();
        this->setKernel(newKernel);
        delete oldKernel;
    }
    if (neighborhoodSearch != nullptr) {
        auto newSearch = this->neighborhoodSearch->CreateCopy(this->getParticleCollection(), getRadius());
        auto oldSearch = this->getNeighborhoodSearch();
        this->setNeighborhoodSearch(newSearch);
        delete oldSearch;
    }
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
}

float FluidSolver::Simulation::getRadius() {
    return 2.0f * particleSize;
}
