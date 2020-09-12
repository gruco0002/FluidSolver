#include "uiVersion/FluidSolverWindow.hpp"
#include "cxxopts.hpp"
#include <iostream>
// #include <FluidSolverConsole.hpp>

void printHelp(cxxopts::Options &options) {
    std::cout << std::endl << options.help(
            {"", "Simulation", "FluidSolver: SimpleSPH", "FluidSolver: IISPH", "Timestep: Constant",
             "Timestep: DynamicCFL"}) << std::endl;
}

int main(int argc, char *argv[]) {

    cxxopts::Options options("FluidSolver", "Simple SPH Fluid Solver tool");
    options.add_options()
            ("h,help", "Help")
            ("c,console",
             "Disable windowed mode and only show in console and use provided arguments. If not given, a gui is shown and no arguments are taken into account.");

    //FluidSolverConsole console(options);

    try {

        auto result = options.parse(argc, argv);


        if (result["help"].as<bool>()) {
            printHelp(options);
            return 0;
        }

        if (result["console"].as<bool>()) {
            // go into console mode

            //console.start(result);
            return 0;
        }

        FluidSolverWindow window("FluidSolver");
        window.MainLoop();
        return 0;
    }
    catch (cxxopts::option_not_exists_exception &exc) {
        std::cout << exc.what() << std::endl;
        printHelp(options);
        return 2;
    }

}