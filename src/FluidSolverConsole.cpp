//
// Created by corbi on 29.05.2019.
//

#include "FluidSolverConsole.hpp"
#include "core/statistics/DataLogger.hpp"
#include <iostream>
#include <core/basicScenarios/BoundaryTestScenario.hpp>
#include <core/basicScenarios/SimpleBoxScenario.hpp>
#include <core/fluidSolver/SPHFluidSolver.hpp>
#include <core/fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch.hpp>
#include <core/fluidSolver/kernel/CubicSplineKernel.hpp>
#include <core/Simulation.hpp>
#include <core/timestep/ConstantTimestep.hpp>
#include <core/fluidSolver/IISPHFluidSolver.hpp>
#include <core/timestep/DynamicCFLTimestep.hpp>

FluidSolverConsole::FluidSolverConsole(cxxopts::Options &options) {
    setupOptions(options);
}

void FluidSolverConsole::start(cxxopts::ParseResult &parseResult) {
    verbose = parseResult["verbose"].as<bool>();

    setupSimulation(parseResult);

    if (verbose)
        std::cout << "Fluid Solver Console Mode" << std::endl;

    if (verbose)
        std::cout << "\tStarting Simulation Process..." << std::endl;
    executeSimulation();

    cleanupSimulation();
}

void FluidSolverConsole::setupOptions(cxxopts::Options &options) {
    // don't forget to mention the help groups in the main.cpp
    options.add_options("Simulation")

            ("scenario", "Scenario selection. Valid options: " + GenerateScenarioOptionList(),
             cxxopts::value<std::string>()->default_value(Scenarios[0]->GetName()))

            ("fluidsolver", "The fluid solver, that should be used for simulating. Valid options: SimpleSPH, IISPH",
             cxxopts::value<std::string>()->default_value("SimpleSPH"))
            ("viscosity", "Viscosity for the simulation", cxxopts::value<float>()->default_value("3.0"))


            ("timestepMode", "Timestep mode for the simulation. Valid options: Constant, DynamicCFL",
             cxxopts::value<std::string>()->default_value("Constant"))


            ("length", "Time length of the simulation in seconds", cxxopts::value<float>()->default_value("60.0"))
            ("output", "Output Logfile for the simulation data",
             cxxopts::value<std::string>()->default_value("log.csv"))
            ("v,verbose", "If set turns on verbose and debug messages", cxxopts::value<bool>());

    options.add_options("FluidSolver: SimpleSPH")
            ("stiffness", "Stiffness (k) for the SimpleSPH Fluid Solver",
             cxxopts::value<float>()->default_value("100000.0"));


    options.add_options("FluidSolver: IISPH")
            ("minIterations", "Minimum number of iterations of the iisph solver",
             cxxopts::value<int>()->default_value("2"))
            ("maxIterations", "Maximum number of iterations of the iisph solver",
             cxxopts::value<int>()->default_value("100"))
            ("maxDensityError", "Maximum density error allowed for the solver",
             cxxopts::value<float>()->default_value("0.001"))
            ("gamma", "Gamma factor for iisph", cxxopts::value<float>()->default_value("0.7"))
            ("omega", "Omega factor for iisph", cxxopts::value<float>()->default_value("0.5"));

    options.add_options("Timestep: Constant")
            ("timestep", "Timestep of the simulation in seconds", cxxopts::value<float>()->default_value("0.001"));

    options.add_options("Timestep: DynamicCFL")
            ("minTimestep", "Minimum allowed timestep of the simulation in seconds",
             cxxopts::value<float>()->default_value("0.0001"))
            ("maxTimestep", "Maximum allowed timestep of the simulation in seconds",
             cxxopts::value<float>()->default_value("0.02"))
            ("cflNumber",
             "CFL Number that should be maintained, if timestep is not constraint by min or max timestep values",
             cxxopts::value<float>()->default_value("0.8"));

}

void FluidSolverConsole::setupSimulation(cxxopts::ParseResult &parseResult) {
    // fetch length of the simulation
    SimulationLength = parseResult["length"].as<float>();

    // create new simulation
    simulation = new FluidSolver::Simulation();
    simulation->setRestDensity(1.0f);
    simulation->setGravity(9.81f);

    // create fluid solver and set its parameters
    std::string solverName = parseResult["fluidsolver"].as<std::string>();
    if (solverName == "SimpleSPH") {
        // simple sph
        auto sphFluidSolver = new FluidSolver::SPHFluidSolver();

        // get values
        float viscosity = parseResult["viscosity"].as<float>();
        float stiffness = parseResult["stiffness"].as<float>();

        // set up values
        sphFluidSolver->StiffnessK = stiffness;
        sphFluidSolver->Viscosity = viscosity;
        simulation->setFluidSolver(sphFluidSolver);

    } else if (solverName == "IISPH") {
        // iisph
        auto iisph = new FluidSolver::IISPHFluidSolver();

        // get values
        float viscosity = parseResult["viscosity"].as<float>();
        int minIterations = parseResult["minIterations"].as<int>();
        int maxIterations = parseResult["maxIterations"].as<int>();
        float maxDensityError = parseResult["maxDensityError"].as<float>();
        float gamma = parseResult["gamma"].as<float>();
        float omega = parseResult["omega"].as<float>();

        // setup values
        iisph->Viscosity = viscosity;
        iisph->MinNumberOfIterations = minIterations;
        iisph->MaxNumberOfIterations = maxIterations;
        iisph->MaxDensityErrorAllowed = maxDensityError;
        iisph->Gamma = gamma;
        iisph->Omega = omega;

        simulation->setFluidSolver(iisph);

    } else {
        if (verbose)
            std::cout << "No valid solver type was given" << std::endl;
        return;
    }

    // create scenario and setup data
    std::string scenarioName = parseResult["scenario"].as<std::string>();
    bool scenarioFound = false;
    for (FluidSolver::Scenario *scenario : Scenarios) {
        if (scenario->GetName() != scenarioName)
            continue;

        // set particle size
        simulation->setParticleSize(scenario->GetParticleSize());

        // set up scenario data, delete old particle collection
        delete simulation->getParticleCollection();
        auto particleCollection = scenario->GenerateScenario(simulation->getRestDensity());
        simulation->setParticleCollection(particleCollection);

        // set up simulation modifiers
        simulation->clearSimulationModifiers();
        for (FluidSolver::ISimulationModifier *mod : scenario->GetSimulationModifiers()) {
            simulation->addSimulationModifier(mod);
        }

        scenarioFound = true;
        break;
    }
    if (!scenarioFound) {
        if (verbose)
            std::cout << "No valid scenario with the given name was found" << std::endl;
        return;
    }

    // set simulation timestep
    std::string timestepMode = parseResult["timestepMode"].as<std::string>();
    if (timestepMode == "Constant") {
        // get timestep value
        float timestepValue = parseResult["timestep"].as<float>();

        // create constant timestep
        auto timestep = new FluidSolver::ConstantTimestep(timestepValue);
        simulation->setTimestep(timestep);

    } else if (timestepMode == "DynamicCFL") {
        // get timestep values
        float minTimestep = parseResult["minTimestep"].as<float>();
        float maxTimestep = parseResult["maxTimestep"].as<float>();
        float cflNumber = parseResult["cflNumber"].as<float>();

        // create dynamic cfl timestep
        auto timestep = new FluidSolver::DynamicCFLTimestep();
        timestep->setCflNumber(cflNumber);
        timestep->setMaximumTimestep(maxTimestep);
        timestep->setMinimumTimestep(minTimestep);
        simulation->setTimestep(timestep);

    } else {
        if (verbose)
            std::cout << "No valid timestep mode with the given name was found" << std::endl;
        return;
    }





    // set statistics collection
    simulation->setStatisticCollector(new FluidSolver::StatisticCollector());


    // setup dataLogger
    std::string outputFile = parseResult["output"].as<std::string>();
    auto dataLogger = new DataLogger(outputFile);
    dataLogger->alwaysFlush = false; // we only want to write at the end
    simulation->setDataLogger(dataLogger);
    simulation->getDataLogger()->StartLogging();

}

void FluidSolverConsole::executeSimulation() {



    // simulate
    float totalTime = 0.0f;
    float lastTimeMessage = 0.0f;
    while (totalTime <= SimulationLength) {
        simulation->ExecuteSimulationStep();
        simulation->CollectStatistics();

        totalTime += simulation->getTimestep()->getCurrentTimestep();

        // messages
        if (totalTime >= lastTimeMessage) {
            if (verbose)
                std::cout << "\tSimulated " << lastTimeMessage << " seconds of the simulation" << std::endl;
            lastTimeMessage += 1;
        }
    }


}

std::string FluidSolverConsole::GenerateScenarioOptionList() {
    auto tmp = std::string();

    for (auto scen : Scenarios) {
        if (!tmp.empty()) {
            tmp.append(", ");
        }
        tmp.append("\"");
        tmp.append(scen->GetName());
        tmp.append("\"");
    }

    return tmp;
}

void FluidSolverConsole::cleanupSimulation() {
    // save logs
    simulation->getDataLogger()->FinishLogging();

    // cleanup
    delete simulation;

}


