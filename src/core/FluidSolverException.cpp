//
// Created by corbi on 24.04.2019.
//

#include <string>
#include "FluidSolverException.hpp"

const char *FluidSolver::FluidSolverException::what() const noexcept {
    return ("FluidSolver Exception: " + text + "\n" + std::string(exception::what())).c_str();
}

FluidSolver::FluidSolverException::FluidSolverException(std::string text) {
    this->text = text;
}
