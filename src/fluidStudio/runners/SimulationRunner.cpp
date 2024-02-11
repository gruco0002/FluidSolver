#include "SimulationRunner.hpp"

#include "Simulator.hpp"

namespace FluidStudio
{

    void SimulationRunner::execute_computation()
    {
        if (simulation == nullptr)
            return;

        LibFluid::CompatibilityReport report;
        simulation->create_compatibility_report(report);
        if (report.has_issues())
        {
            report.log_issues();
            return;
        }

        simulation->execute_simulation_step();
    }

    SimulationRunner::SimulationRunner()
    {
        run_asynchronously = true; // by default we want the simulation to run asynchronously
    }
} // namespace FluidStudio
