//
// Created by corbi on 29.05.2019.
//

#include "FluidSolverConsole.hpp"
#include <iostream>

FluidSolverConsole::FluidSolverConsole(cxxopts::Options &options) {
    setupOptions(options);
}

void FluidSolverConsole::start(cxxopts::ParseResult &parseResult) {
    std::cout << "Fluid Solver Console Mode" << std::endl;
}

void FluidSolverConsole::setupOptions(cxxopts::Options &options) {
    options.add_options("Simulation")

            ("s,scenario", "Scenario selection", cxxopts::value<std::string>()->default_value("box"))
            ("t,timestep", "Timestep for the simulation in seconds", cxxopts::value<float>()->default_value("0.001"))
            ("k,stiffness", "Stiffness (k) for the simulation", cxxopts::value<float>()->default_value("100000.0"))
            ("v,viscosity", "Viscosity for the simulation", cxxopts::value<float>()->default_value("3.0"))
            ("l,length", "Time length of the simulation in seconds", cxxopts::value<float>()->default_value("60.0"))
            ("o,output", "Output Logfile for the simulation data", cxxopts::value<std::string>()->default_value("log.csv"));
}


