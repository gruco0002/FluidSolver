#include "ITimestep.hpp"

void FluidSolver::ITimestep::setParticleCollection(FluidSolver::ParticleCollection *particleCollection) {
    this->particleCollection = particleCollection;
}

FluidSolver::ParticleCollection *FluidSolver::ITimestep::getParticleCollection() {
    return this->particleCollection;
}

FluidSolver::ITimestep::~ITimestep() {

}

float FluidSolver::ITimestep::getParticleSize() const {
    return ParticleSize;
}

void FluidSolver::ITimestep::setParticleSize(float particleSize) {
    ParticleSize = particleSize;
}
