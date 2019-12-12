//
// Created by Corbinian Gruber on 12.12.19.
//

#include "ITimestep.hpp"

void FluidSolver::ITimestep::setParticleCollection(FluidSolver::IParticleCollection *particleCollection) {
    this->particleCollection = particleCollection;
}

FluidSolver::IParticleCollection *FluidSolver::ITimestep::getParticleCollection() {
    return this->particleCollection;
}

FluidSolver::ITimestep::~ITimestep() {

}
