//
// Created by corbi on 18.12.2019.
//

#include "ContinousVisualizer.hpp"

void FluidSolver::ContinousVisualizer::setParticleCollection(FluidSolver::IParticleCollection *particleCollection) {
    ParticleCollection = particleCollection;
}

FluidSolver::IParticleCollection *FluidSolver::ContinousVisualizer::getParticleCollection() {
    return ParticleCollection;
}

void FluidSolver::ContinousVisualizer::setParticleSize(float particleSize) {
    ParticleSize = particleSize;
}

float FluidSolver::ContinousVisualizer::getParticleSize() {
    return ParticleSize;
}

void FluidSolver::ContinousVisualizer::Render() {

}

float FluidSolver::ContinousVisualizer::getRestDensity() {
    return RestDensity;
}

void FluidSolver::ContinousVisualizer::setRestDensity(float restDensity) {
    RestDensity = restDensity;
}
