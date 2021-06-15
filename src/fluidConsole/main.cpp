#include "FluidSolverConsole.hpp"
#include "Log.hpp"
#include "cxxopts.hpp"

#include <iostream>

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

    cxxopts::Options options("FluidSolver", "SPH Fluid Solver Console Application");
    options.add_options()("h,help", "Help");

    // initialize the console
    FluidSolverConsole console(options);

    try
    {
        // parse the parameters
        auto result = options.parse(argc, argv);

        if (result["help"].as<bool>())
        {
            printHelp(options);
            return 0;
        }

        // start the console mode
        console.start(result);
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