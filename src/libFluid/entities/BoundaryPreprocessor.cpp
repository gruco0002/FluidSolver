#include "BoundaryPreprocessor.hpp"

#include "LibFluidMath.hpp"

namespace LibFluid {

    void BoundaryPreprocessor::execute_simulation_step(const Timepoint& timepoint, bool before_solver) {
        FLUID_ASSERT(before_solver, "It makes only sense to execute the preprocessor before the solver!");

        initialize();


        auto& collection = simulation_data.collection;
        auto& interface = simulation_data.neighborhood_interface;

        FLUID_ASSERT(collection != nullptr);
        FLUID_ASSERT(interface != nullptr);
        FLUID_ASSERT(collection->is_type_present<MovementData3D>());
        FLUID_ASSERT(collection->is_type_present<ParticleInfo>());
        FLUID_ASSERT(collection->is_type_present<ParticleData>());

        for (size_t i = 0; i < collection->size(); i++) {
            const auto& info = collection->get<ParticleInfo>(i);
            if (info.type != ParticleType::ParticleTypeBoundary) {
                continue;
            }

            auto& data = collection->get<ParticleData>(i);
            const auto& mv = collection->get<MovementData3D>(i);
            float volume_reciprocal = 0.0f;

            auto neighbors = interface->get_neighbors(i);
            for (const auto& neighbor : neighbors) {
                const auto& neighbor_info = collection->get<ParticleInfo>(neighbor);
                if (neighbor_info.type != ParticleType::ParticleTypeBoundary) {
                    continue;
                }

                const auto& neighbor_data = collection->get<ParticleData>(neighbor);
                const auto& neighbor_mv = collection->get<MovementData3D>(neighbor);

                volume_reciprocal += kernel.GetKernelValue(neighbor_mv.position, mv.position);
            }

            float corrected_mass = simulation_data.rest_density * gamma_1 / volume_reciprocal;
            data.mass = Math::min(max_allowed_mass, corrected_mass);
        }
    }

    void BoundaryPreprocessor::initialize() {
        if (settings.has_data_changed()) {
            settings.acknowledge_data_change();
        }
        if (simulation_data.has_data_changed()) {
            simulation_data.acknowledge_data_change();

            kernel.kernel_support = simulation_data.particle_size * Constants::kernel_support_factor;
            kernel.initialize();

            ideal_volume_reciprocal = calculate_ideal_volume_reciprocal();
            gamma_1 = calculate_gamma_1_from_ideal_volume_reciprocal();
            max_allowed_mass = calculate_max_allowed_mass();
        }
    }

    void BoundaryPreprocessor::create_compatibility_report(CompatibilityReport& report) {
        initialize();

        report.begin_scope(FLUID_NAMEOF(BoundaryPreprocessor));

        if (simulation_data.collection == nullptr) {
            report.add_issue("Particle collection is null!");
        } else {
            if (!simulation_data.collection->is_type_present<MovementData3D>()) {
                report.add_issue("Particles are missing the MovementData3D attribute!");
            }
            if (!simulation_data.collection->is_type_present<ParticleData>()) {
                report.add_issue("Particles are missing the ParticleData attribute!");
            }
            if (!simulation_data.collection->is_type_present<ParticleInfo>()) {
                report.add_issue("Particles are missing the ParticleInfo attribute!");
            }
        }

        if (simulation_data.particle_size <= 0.0f) {
            report.add_issue("Particle size is smaller or equal to zero!");
        }

        report.end_scope();
    }

    BoundaryPreprocessor::BoundaryPreprocessor() {
        settings.execution_point = EntityExecutionPoint::BeforeSolver;
    }

    float BoundaryPreprocessor::calculate_ideal_volume_reciprocal() const {
        float volume_reciprocal = 0.0f;

        // calculating the volume of a perfectly sampled plane
        for (int x = 0; x <= 0; x++) {
            for (int y = -2; y <= 2; y++) {
                for (int z = -2; z <= 2; z++) {
                    const auto position = glm::vec3((float)x, (float)y, (float)z) * simulation_data.particle_size;
                    volume_reciprocal += kernel.GetKernelValue(position);
                }
            }
        }

        return volume_reciprocal;
    }
    float BoundaryPreprocessor::calculate_gamma_1_from_ideal_volume_reciprocal() const {
        float gamma_1_calc = Math::pow3(simulation_data.particle_size) * ideal_volume_reciprocal;
        return gamma_1_calc;
    }
    float BoundaryPreprocessor::calculate_max_allowed_mass() const {
        return simulation_data.rest_density * Math::pow3(simulation_data.particle_size);
    }
} // namespace LibFluid
