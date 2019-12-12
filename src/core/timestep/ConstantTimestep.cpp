//
// Created by Corbinian Gruber on 12.12.19.
//

#include "ConstantTimestep.hpp"

void FluidSolver::ConstantTimestep::CalculateCurrentTimestep() {
    // do nothing
}

float FluidSolver::ConstantTimestep::getCurrentTimestep() {
    return timestep;
}

float FluidSolver::ConstantTimestep::getTimestep() const {
    return timestep;
}

void FluidSolver::ConstantTimestep::setTimestep(float timestep) {
    ConstantTimestep::timestep = timestep;
}

FluidSolver::ConstantTimestep::ConstantTimestep(float timestep) : timestep(timestep) {}
