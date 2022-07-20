#include "ParticleRemover.hpp"

#include "group/AreaGroup.hpp"
#include "group/GroupActions.hpp"


namespace FluidSolver {
    void ParticleRemover::execute_simulation_step(pFloat timestep) {
        AreaGroup group;
        group.area = parameters.area;
        group.collection = sim.collection;
        group.inside = !parameters.remove_if_outside;
        GroupActions::remove(group);
    }
    void ParticleRemover::initialize() {

    }
    void ParticleRemover::create_compatibility_report(CompatibilityReport& report) {

    }


} // namespace FluidSolver
