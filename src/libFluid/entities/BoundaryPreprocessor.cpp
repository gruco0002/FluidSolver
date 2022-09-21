#include "BoundaryPreprocessor.hpp"

#include "Log.hpp"

namespace LibFluid {

    void BoundaryPreprocessor::execute_simulation_step(const Timepoint& timepoint, bool before_solver) {
        FLUID_ASSERT(before_solver, "It makes only sense to execute the preprocessor before the solver!");

        Log::warning("Test");
    }

    void BoundaryPreprocessor::initialize() {
        if (settings.has_data_changed()) {
            settings.acknowledge_data_change();
        }
        if (simulation_data.has_data_changed()) {
            simulation_data.acknowledge_data_change();
        }
    }

    void BoundaryPreprocessor::create_compatibility_report(CompatibilityReport& report) {
    }

    BoundaryPreprocessor::BoundaryPreprocessor() {
        settings.execution_point = EntityExecutionPoint::BeforeSolver;
    }
} // namespace LibFluid
