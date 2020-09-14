#include "Simulation.hpp"

bool FluidSolver::Simulation::SimulationParameters::operator==(
        const FluidSolver::Simulation::SimulationParameters &other) const {
    return other.fluid_solver == fluid_solver
           && other.collection == collection
           && other.particle_size == particle_size
           && other.rest_density == rest_density
           && other.timestep == timestep
           && other.gravity == gravity
           && other.visualizer == visualizer
           && other.invalidate == invalidate;
}

bool FluidSolver::Simulation::SimulationParameters::operator!=(
        const FluidSolver::Simulation::SimulationParameters &other) const {
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
    internal_parameters.timestep->CalculateCurrentTimestep();
    float current_timestep = internal_parameters.timestep->getCurrentTimestep();

    // simulate
    internal_parameters.fluid_solver->execute_simulation_step(current_timestep);


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
        internal_parameters.timestep->setParticleSize(internal_parameters.particle_size);
        internal_parameters.timestep->setParticleCollection(internal_parameters.collection);
    }

}

void FluidSolver::Simulation::visualize() {

    if (internal_parameters != parameters) {
        initialize();
    }

    FLUID_ASSERT(internal_parameters.visualizer != nullptr)
    FLUID_ASSERT(internal_parameters.collection != nullptr)

    internal_parameters.visualizer->Render();

}
