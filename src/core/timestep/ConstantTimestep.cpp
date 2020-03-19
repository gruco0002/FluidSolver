#include "ConstantTimestep.hpp"

void FluidSolver::ConstantTimestep::CalculateCurrentTimestep() {
    // do nothing
}

float FluidSolver::ConstantTimestep::getCurrentTimestep() {
    return Timestep;
}

float FluidSolver::ConstantTimestep::getTimestep() const {
    return Timestep;
}

void FluidSolver::ConstantTimestep::setTimestep(float timestep) {
    ConstantTimestep::Timestep = Timestep;
}

FluidSolver::ConstantTimestep::ConstantTimestep(float timestep) : Timestep(timestep) {}
