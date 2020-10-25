#include "ScenarioData.hpp"

FluidSolver::ScenarioData::~ScenarioData() {
    for (auto ptr:entities)
        delete ptr;
}
