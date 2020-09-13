#include "Scenario.hpp"

FluidSolver::Scenario::Scenario(const std::string &filepath) : filepath(filepath) {
    setup();
}

void FluidSolver::Scenario::setup() {
    interface.make_available(chai);
    chai.eval_file(filepath);
}


FluidSolver::ScenarioData *FluidSolver::Scenario::create_data() {
    auto data = new ScenarioData();
    interface.current = data;

    data->collection.add_type<MovementData>();
    data->collection.add_type<ParticleData>();
    data->collection.add_type<ParticleInfo>();
    data->collection.add_type<ExternalForces>();

    // load the particles
    try {
        chai.eval("loadParticles()");
        data->rest_density = chai.eval<double>("getRestDensity()");
        data->particle_size = chai.eval<double>("getParticleSize()");
    } catch (const chaiscript::exception::eval_error &e) {
        std::cerr << "Error\n" << e.pretty_print() << std::endl;
    }

    interface.current = nullptr;
    return data;
}

