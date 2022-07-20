#include "ParticleRemover3D.hpp"

#include "group/GroupActions.hpp"
#include "group/VolumeGroup.hpp"

namespace FluidSolver {

    void ParticleRemover3D::execute_simulation_step(pFloat timestep) {
        VolumeGroup group;
        group.volume = parameters.volume;
        group.collection = sim.collection;
        group.inside = !parameters.remove_if_outside;
        GroupActions::remove(group);
    }
    void ParticleRemover3D::initialize() {
    }
    void ParticleRemover3D::create_compatibility_report(CompatibilityReport& report) {
    }
} // namespace FluidSolver