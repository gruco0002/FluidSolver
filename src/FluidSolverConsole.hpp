//
// Created by corbi on 29.05.2019.
//

#ifndef FLUIDSOLVER_FLUIDSOLVERCONSOLE_HPP
#define FLUIDSOLVER_FLUIDSOLVERCONSOLE_HPP

#include <core/basicScenarios/Scenario.hpp>
#include "libraries/cxxopts/cxxopts.hpp"

class FluidSolverConsole {

public:
    explicit FluidSolverConsole(cxxopts::Options &options);

    void start(cxxopts::ParseResult &parseResult);

private:
    void setupOptions(cxxopts::Options &options);

    void setVariables(cxxopts::ParseResult &parseResult);

    void executeSimulation();

    FluidSolver::Scenario *getScenario();


    std::string scenario;
    float timestep;
    float stiffness;
    float viscosity;
    float length;
    std::string output;
    bool verbose;

};


#endif //FLUIDSOLVER_FLUIDSOLVERCONSOLE_HPP
