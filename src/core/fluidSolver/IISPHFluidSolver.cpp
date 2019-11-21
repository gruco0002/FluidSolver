//
// Created by corbi on 20.11.2019.
//

#include "IISPHFluidSolver.hpp"

void FluidSolver::IISPHFluidSolver::ExecuteSimulationStep() {

}

float FluidSolver::IISPHFluidSolver::getParticleSize() {
    return ParticleSize;
}

void FluidSolver::IISPHFluidSolver::setParticleSize(float particleSize) {
    this->ParticleSize = particleSize;
}

float FluidSolver::IISPHFluidSolver::getRestDensity() {
    return RestDensity;
}

void FluidSolver::IISPHFluidSolver::setRestDensity(float restDensity) {
    this->RestDensity = restDensity;
}

float FluidSolver::IISPHFluidSolver::getTimestep() {
    return Timestep;
}

void FluidSolver::IISPHFluidSolver::setTimestep(float timestep) {
    this->Timestep = timestep;
}

void FluidSolver::IISPHFluidSolver::setParticleCollection(FluidSolver::IParticleCollection *particleCollection) {
    this->ParticleCollection = particleCollection;
}

FluidSolver::IParticleCollection *FluidSolver::IISPHFluidSolver::getParticleCollection() {
    return ParticleCollection;
}
