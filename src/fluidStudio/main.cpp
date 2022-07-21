#include "FluidSolverWindow.hpp"

int main(int argc, char** args)
{
    FluidStudio::FluidSolverWindow window("FluidStudio");
    window.main_loop();
    return 0;
}
