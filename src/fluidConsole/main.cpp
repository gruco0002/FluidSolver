
#include "Log.hpp"
#include "Simulation.hpp"
#include "serialization/SimulationSerializer.hpp"

#include <cxxopts.hpp>
#include <filesystem>
#include <iostream>
#include <string>


void printHelp(cxxopts::Options& options)
{
    std::cout << std::endl
              << options.help({
                     "",
                     "Console",
                 })
              << std::endl;
}

int main(int argc, char* argv[])
{
    FluidSolver::Log::print_to_console = true;


    cxxopts::Options options("FluidConsole", "SPH Fluid Solver console application. Allows simulating scenarios "
                                             "without a user interface or input by the user.");
    options.add_options()("h,help", "Show help and information about the FluidConsole application.");
    options.add_options("Console")("l,length", "Amount of time (in seconds) the scenario should be simulated.",
                                   cxxopts::value<float>()->default_value("60.0"))(
        "v,verbose", "If this flag is provided more information is printed to the console",
        cxxopts::value<bool>())("f,file", "[Required] Filepath to a scenario yaml file", cxxopts::value<std::string>());


    try
    {
        // parse the parameters
        auto result = options.parse(argc, argv);

        if (result["help"].as<bool>())
        {
            printHelp(options);
            return 0;
        }

        struct
        {
            float simulation_length = 1.0f;
            bool verbose = false;
            std::string filepath = "";
        } settings;

        try
        {
            settings.simulation_length = result["length"].as<float>();
            settings.verbose = result["verbose"].as<bool>();
            settings.filepath = result["file"].as<std::string>();
        }
        catch (const std::exception& e)
        {
            FluidSolver::Log::error("[Console] Invalid or missing arguments: " + std::string(e.what()));
            return 5;
        }

        if (!std::filesystem::exists(settings.filepath))
        {
            FluidSolver::Log::error("[Console] Specified simulation file does not exist!");
            return 6;
        }


        if (settings.verbose)
            FluidSolver::Log::message("[Console] Starting in console mode.");

        // Load file
        FluidSolver::SimulationSerializer s(settings.filepath);
        FluidSolver::Simulation simulation = s.load_from_file();
        if (s.has_errors())
        {
            FluidSolver::Log::error("[Console] Loading of scenario caused errors!");
            return 3;
        }

        // check compatibility
        if (settings.verbose)
            FluidSolver::Log::message("[Console] Checking if components are compatibile with each other.");

        simulation.manual_initialize();
        auto compatibility_report = simulation.check();
        if (compatibility_report.has_issues())
        {
            compatibility_report.log_issues();
            return 4;
        }


        // start simulating
        if (settings.verbose)
            FluidSolver::Log::message("[Console] Starting simulation process.");

        // simulate
        float last_time_message = 0.0f;
        while (simulation.get_current_timepoint().simulation_time <= settings.simulation_length)
        {
            simulation.execute_simulation_step();

            // informational log messages
            if (simulation.get_current_timepoint().simulation_time >= last_time_message)
            {
                if (settings.verbose)
                    FluidSolver::Log::message("[Console] Simulated " + std::to_string(last_time_message) +
                                              " seconds of the simulation.");
                last_time_message += 1.0f;
            }
        }

        // manual save of the output
        simulation.parameters.output.manual_save();

        FluidSolver::Log::message("[Console] Simulation has finished.");


        return 0;
    }
    catch (cxxopts::option_not_exists_exception& exc)
    {
        FluidSolver::Log::print_to_console = true;
        FluidSolver::Log::error(exc.what());
        printHelp(options);
        return 2;
    }
}