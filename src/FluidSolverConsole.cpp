//
// Created by corbi on 29.05.2019.
//

#include "FluidSolverConsole.hpp"
#include "DataLogger.hpp"
#include <iostream>
#include <core/basicScenarios/BoundaryTestScenario.hpp>
#include <core/basicScenarios/SimpleBoxScenario.hpp>
#include <core/fluidSolver/SPHFluidSolver.hpp>
#include <core/fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch.hpp>
#include <core/fluidSolver/kernel/CubicSplineKernel.hpp>
#include <core/Simulation.hpp>

FluidSolverConsole::FluidSolverConsole(cxxopts::Options &options) {
    setupOptions(options);
}

void FluidSolverConsole::start(cxxopts::ParseResult &parseResult) {
    setVariables(parseResult);

    if (verbose)
        std::cout << "Fluid Solver Console Mode" << std::endl;

    if (verbose)
        std::cout << "\tStarting Simulation Process..." << std::endl;
    executeSimulation();
}

void FluidSolverConsole::setupOptions(cxxopts::Options &options) {
    options.add_options("Simulation")

            ("s,scenario", "Scenario selection. Valid options: box, boundaryTest",
             cxxopts::value<std::string>()->default_value("box"))
            ("t,timestep", "Timestep for the simulation in seconds", cxxopts::value<float>()->default_value("0.001"))
            ("k,stiffness", "Stiffness (k) for the simulation", cxxopts::value<float>()->default_value("100000.0"))
            ("v,viscosity", "Viscosity for the simulation", cxxopts::value<float>()->default_value("3.0"))
            ("l,length", "Time length of the simulation in seconds", cxxopts::value<float>()->default_value("60.0"))
            ("o,output", "Output Logfile for the simulation data",
             cxxopts::value<std::string>()->default_value("log.csv"))
            ("d,debug", "If set turns on debug messages", cxxopts::value<bool>());
}

void FluidSolverConsole::setVariables(cxxopts::ParseResult &parseResult) {
    scenario = parseResult["scenario"].as<std::string>();
    timestep = parseResult["timestep"].as<float>();
    viscosity = parseResult["viscosity"].as<float>();
    length = parseResult["length"].as<float>();
    output = parseResult["output"].as<std::string>();
    verbose = parseResult["debug"].as<bool>();
    stiffness = parseResult["stiffness"].as<float>();
}

void FluidSolverConsole::executeSimulation() {
    // first get scenario
    FluidSolver::Scenario *scenario = getScenario();
    if (!scenario) {
        if (verbose)
            std::cout << "No valid scenario was found" << std::endl;
        return;
    }

   auto simulation = new FluidSolver::Simulation();

    // set particle size and timestep
    simulation->setTimestep(timestep);
    simulation->setParticleSize(scenario->GetParticleSize());
    simulation->setRestDensity(1.0f);
    simulation->setGravity(9.81f);

    // set up basic stuff
    auto sphFluidSolver = new FluidSolver::SPHFluidSolver();

    // set up values
    sphFluidSolver->KernelSupport = 2.0f * simulation->getParticleSize();
    sphFluidSolver->NeighborhoodRadius = 2.0f * simulation->getParticleSize();
    sphFluidSolver->StiffnessK =stiffness;
    sphFluidSolver->Viscosity =viscosity;
    sphFluidSolver->kernel = new FluidSolver::CubicSplineKernel();
    sphFluidSolver->neighborhoodSearch = new FluidSolver::HashedNeighborhoodSearch(simulation->getParticleSize() * 3);
    simulation->setFluidSolver(sphFluidSolver);


    // set up scenario data
    auto particleCollection = scenario->GenerateScenario(simulation->getRestDensity());
    simulation->setParticleCollection(particleCollection);
    for (FluidSolver::ISimulationModifier *mod : scenario->GetSimulationModifiers()) {
        simulation->addSimulationModifier(mod);
    }

    // set statistics collection
    simulation->setStatisticCollector(new FluidSolver::StatisticCollector());


    // setup dataLogger
    auto dataLogger = new DataLogger(output);
    dataLogger->alwaysFlush = false; // we only want to write at the end
    simulation->setDataLogger(dataLogger);
    dataLogger->StartLogging();


    // simulate
    float totalTime = 0.0f;
    float lastTimeMessage = 0.0f;
    while (totalTime <= length) {
        totalTime += simulation->getTimestep();
        simulation->ExecuteSimulationStep();
        simulation->CollectStatistics();

        // messages
        if (totalTime >= lastTimeMessage) {
            if (verbose)
                std::cout << "\tSimulated " << lastTimeMessage << " seconds of the simulation" << std::endl;
            lastTimeMessage += 1;
        }
    }

    // save logs
    dataLogger->FinishLogging();

    // cleanup
    delete dataLogger;
    delete sphFluidSolver;
    delete simulation;
    // TODO: proper cleanup


}

FluidSolver::Scenario *FluidSolverConsole::getScenario() {
    if (scenario == "box") {
        return new FluidSolver::SimpleBoxScenario();
    } else if (scenario == "boundaryTest") {
        return new FluidSolver::BoundaryTestScenario();
    } else {
        return nullptr;
    }
}


