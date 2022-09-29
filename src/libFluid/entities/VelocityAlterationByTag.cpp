#include "VelocityAlterationByTag.hpp"

namespace LibFluid {


    void VelocityAlterationByTag::execute_simulation_step(const Timepoint& timepoint, bool before_solver) {
        FLUID_ASSERT(before_solver, "This entity should set the velocity before the solver integrates the particles!");

        initialize();

        auto& collection = simulation_data.collection;

        FLUID_ASSERT(collection != nullptr);
        FLUID_ASSERT(collection->is_type_present<MovementData3D>());
        FLUID_ASSERT(collection->is_type_present<ParticleInfo>());

        for (size_t i = 0; i < collection->size(); i++) {
            const auto& info = collection->get<ParticleInfo>(i);
            if (info.tag != parameters.tag) {
                continue;
            }

            auto& mv = collection->get<MovementData3D>(i);
            mv.velocity = parameters.velocity;
        }
    }

    void VelocityAlterationByTag::initialize() {
        if (settings.has_data_changed()) {
            settings.acknowledge_data_change();
        }
        if (simulation_data.has_data_changed()) {
            simulation_data.acknowledge_data_change();
        }
        if (parameters.has_data_changed()) {
            parameters.acknowledge_data_change();
        }
    }

    void VelocityAlterationByTag::create_compatibility_report(CompatibilityReport& report) {
        initialize();

        report.begin_scope(FLUID_NAMEOF(VelocityAlterationByTag));

        if (simulation_data.collection == nullptr) {
            report.add_issue("Particle collection is null!");
        } else {
            if (!simulation_data.collection->is_type_present<MovementData3D>()) {
                report.add_issue("Particles are missing the MovementData3D attribute!");
            }
            if (!simulation_data.collection->is_type_present<ParticleInfo>()) {
                report.add_issue("Particles are missing the ParticleInfo attribute!");
            }
        }


        report.end_scope();
    }

    VelocityAlterationByTag::VelocityAlterationByTag() {
        settings.execution_point = EntityExecutionPoint::BeforeSolver;
    }
} // namespace LibFluid