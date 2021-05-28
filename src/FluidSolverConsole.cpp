#include "FluidSolverConsole.hpp"

#include "core/Log.hpp"
#include "core/serialization/SimulationSerializer.hpp"

FluidSolverConsole::FluidSolverConsole(cxxopts::Options& options)
{
    setup_options(options);
}

void FluidSolverConsole::start(const cxxopts::ParseResult& parseResult)
{
    FluidSolver::Log::print_to_console = true;

    // read the settings
    read_settings(parseResult);


    if (settings.verbose)
        FluidSolver::Log::message("[Console] Starting in console mode.");

    // Load file
    FluidSolver::SimulationSerializer s(settings.filepath);
    auto simulation = s.load_from_file();
    if (s.has_errors())
    {
        FluidSolver::Log::error("[Console] Loading of scenario caused errors!");
        return;
    }


    // start simulating
    if (settings.verbose)
        FluidSolver::Log::message("[Console] Starting simulation process.");

    // simulate
    float total_time = 0.0f;
    float last_time_message = 0.0f;
    while (total_time <= settings.simulation_length)
    {
        simulation.execute_simulation_step();
        total_time += simulation.parameters.timestep->get_current_timestep();

        // informational log messages
        if (total_time >= last_time_message)
        {
            if (settings.verbose)
                FluidSolver::Log::message("[Console] Simulated " + std::to_string(last_time_message) +
                                          " seconds of the simulation.");
            last_time_message += 1.0f;
        }
    }

    // Output and cleanup
    // TODO: output data


    FluidSolver::Log::message("[Console] Simulation has finished.");
}

void FluidSolverConsole::setup_options(cxxopts::Options& options)
{
    options.add_options("Console")("length", "Time length of the simulation in seconds",
                                   cxxopts::value<float>()->default_value("60.0"))(
        "v,verbose", "If set turns on verbose and debug messages", cxxopts::value<bool>())(
        "f,file", "Required in console mode: Scenario yaml file", cxxopts::value<std::string>());
}

void FluidSolverConsole::read_settings(const cxxopts::ParseResult& parseResult)
{
    settings.simulation_length = parseResult["length"].as<float>();
    settings.verbose = parseResult["verbose"].as<bool>();
    settings.filepath = parseResult["file"].as<std::string>();
}
