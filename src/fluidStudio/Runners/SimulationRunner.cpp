#include "SimulationRunner.hpp"

#include "Simulation.hpp"

namespace FluidUi {

    void SimulationRunner::execute_computation() {
        if (simulation == nullptr)
            return;

        simulation->check_for_initialization();

        auto compatibility_report = simulation->check();
        if (compatibility_report.has_issues()) {
            compatibility_report.log_issues();
            return;
        }

        simulation->execute_simulation_step();
    }

    SimulationRunner::SimulationRunner() {
        run_asynchronously = true; // by default we want the simulation to run asynchronously
    }
} // namespace FluidUi