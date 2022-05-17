#include "SimulationRunner.hpp"

#include "Simulator.hpp"

namespace FluidUi {

    void SimulationRunner::execute_computation() {
        if (simulation == nullptr)
            return;

        FluidSolver::CompatibilityReport report;
        simulation->create_compatibility_report(report);
        if (report.has_issues()) {
            report.log_issues();
            return;
        }

        simulation->execute_simulation_step();
    }

    SimulationRunner::SimulationRunner() {
        run_asynchronously = true; // by default we want the simulation to run asynchronously
    }
} // namespace FluidUi