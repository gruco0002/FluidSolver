#include "Scenario.hpp"

FluidSolver::Scenario::Scenario(const std::string &filepath) : filepath(filepath) {
    setup();
}

void FluidSolver::Scenario::setup() {
    interface.data = &data;

    data.collection.add_type<MovementData>();
    data.collection.add_type<ParticleData>();
    data.collection.add_type<ParticleInfo>();
    data.collection.add_type<ExternalForces>();

    interface.load_file(filepath);
}

