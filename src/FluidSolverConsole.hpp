//
// Created by corbi on 29.05.2019.
//

#ifndef FLUIDSOLVER_FLUIDSOLVERCONSOLE_HPP
#define FLUIDSOLVER_FLUIDSOLVERCONSOLE_HPP

#include <core/basicScenarios/Scenario.hpp>
#include "libraries/cxxopts/cxxopts.hpp"

#include <core/Simulation.hpp>

#include <core/basicScenarios/Scenario.hpp>
#include <core/basicScenarios/HugeDamScenario.hpp>
#include <core/basicScenarios/SimpleBoxScenario.hpp>
#include <core/basicScenarios/MultipleHoleScenario.hpp>
#include <core/basicScenarios/BoxWithHoleScenario.hpp>
#include <core/basicScenarios/SimpleBoxScenarioSmallerParticles.hpp>
#include <core/basicScenarios/SimpleDamScenario.hpp>
#include <core/basicScenarios/BoundaryTestScenario.hpp>
#include <core/basicScenarios/SimpleBoxScenarioLargerParticles.hpp>

class FluidSolverConsole {

public:
    explicit FluidSolverConsole(cxxopts::Options &options);

    void start(cxxopts::ParseResult &parseResult);

private:
    void setupOptions(cxxopts::Options &options);

    void setupSimulation(cxxopts::ParseResult &parseResult);

    void executeSimulation();

    void cleanupSimulation();

    std::string GenerateScenarioOptionList();

    std::vector<FluidSolver::Scenario *> Scenarios = std::vector<FluidSolver::Scenario *>({
                                                                                                  new FluidSolver::SimpleBoxScenario(),
                                                                                                  new FluidSolver::BoxWithHoleScenario(),
                                                                                                  new FluidSolver::MultipleHoleScenario(),
                                                                                                  new FluidSolver::SimpleBoxScenarioSmallerParticles(),
                                                                                                  new FluidSolver::SimpleBoxScenarioLargerParticles(),
                                                                                                  new FluidSolver::SimpleDamScenario(),
                                                                                                  new FluidSolver::HugeDamScenario(),
                                                                                                  new FluidSolver::BoundaryTestScenario(),
                                                                                          }

    );

    FluidSolver::Simulation *simulation = nullptr;

    bool verbose = false;

    float SimulationLength = 1.0f;

};


#endif //FLUIDSOLVER_FLUIDSOLVERCONSOLE_HPP
