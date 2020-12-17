#include "Simulation.hpp"

bool FluidSolver::SimulationParameters::operator==(
        const FluidSolver::SimulationParameters &other) const {
    return other.fluid_solver == fluid_solver
           && other.collection == collection
           && other.particle_size == particle_size
           && other.rest_density == rest_density
           && other.timestep == timestep
           && other.gravity == gravity
           && other.visualizer == visualizer
           && other.invalidate == invalidate
           && other.entities == entities
           && other.sensor_storage == sensor_storage
           && other.sensors == sensors;
}

bool FluidSolver::SimulationParameters::operator!=(
        const FluidSolver::SimulationParameters &other) const {
    return !(*this == other);
}

void FluidSolver::Simulation::execute_simulation_step() {

    if (internal_parameters != parameters) {
        initialize();
    }

    FLUID_ASSERT(internal_parameters.fluid_solver != nullptr)
    FLUID_ASSERT(internal_parameters.collection != nullptr)
    FLUID_ASSERT(internal_parameters.timestep != nullptr)
    FLUID_ASSERT(internal_parameters.rest_density > 0.0f)
    FLUID_ASSERT(internal_parameters.particle_size > 0.0f)


    // calculate timestep
    internal_parameters.timestep->calculate_current_timestep();
    float current_timestep = internal_parameters.timestep->get_current_timestep();

    // simulate
    internal_parameters.fluid_solver->execute_simulation_step(current_timestep);

    // simulate entities
    for (auto ent: internal_parameters.entities) {
        ent->execute_simulation_step(current_timestep);
    }

    // measure sensor data
    if (internal_parameters.sensor_storage != nullptr) {
        internal_parameters.sensor_storage->simulation_time += current_timestep;
    }
    for (auto sen: internal_parameters.sensors) {
        sen->calculate_and_store(current_timestep);
    }

}

void FluidSolver::Simulation::initialize() {
    FLUID_ASSERT(parameters.collection != nullptr)
    FLUID_ASSERT(parameters.rest_density > 0.0f)
    FLUID_ASSERT(parameters.particle_size > 0.0f)

    internal_parameters = parameters;
    internal_parameters.invalidate = false;
    parameters.invalidate = false;

    if (internal_parameters.fluid_solver != nullptr) {
        internal_parameters.fluid_solver->parameters.particle_size = parameters.particle_size;
        internal_parameters.fluid_solver->parameters.rest_density = parameters.rest_density;
        internal_parameters.fluid_solver->parameters.gravity = parameters.gravity;
        internal_parameters.fluid_solver->collection = parameters.collection;
        internal_parameters.fluid_solver->initialize();
    }

    if (internal_parameters.visualizer != nullptr) {
        internal_parameters.visualizer->setParticleCollection(internal_parameters.collection);
        internal_parameters.visualizer->setParticleSize(internal_parameters.particle_size);
        internal_parameters.visualizer->setRestDensity(internal_parameters.rest_density);
    }

    if (internal_parameters.timestep != nullptr) {
        internal_parameters.timestep->parameters.particle_size = internal_parameters.particle_size;
        internal_parameters.timestep->parameters.particle_collection = internal_parameters.collection;
    }

    for (auto ent: internal_parameters.entities) {
        FLUID_ASSERT(ent != nullptr)
        ent->sim.collection = internal_parameters.collection;
        ent->sim.gravity = internal_parameters.gravity;
        ent->sim.particle_size = internal_parameters.particle_size;
        ent->initialize();
    }

    for (auto sen: internal_parameters.sensors) {
        FLUID_ASSERT(internal_parameters.sensor_storage != nullptr)
        FLUID_ASSERT(sen != nullptr)
        sen->simulation_parameters = &internal_parameters;
        sen->storage = internal_parameters.sensor_storage;
        sen->initialize();
    }

}

void FluidSolver::Simulation::visualize(bool update_data) {

    if (internal_parameters != parameters) {
        initialize();
    }

    FLUID_ASSERT(internal_parameters.visualizer != nullptr)
    FLUID_ASSERT(internal_parameters.collection != nullptr)

    if (update_data) {
        internal_parameters.visualizer->UpdateData();
    }
    internal_parameters.visualizer->Render();

}
