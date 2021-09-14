#include "Simulation.hpp"

bool FluidSolver::SimulationParameters::operator==(const FluidSolver::SimulationParameters& other) const
{
    return other.fluid_solver == fluid_solver && other.collection == collection &&
           other.particle_size == particle_size && other.rest_density == rest_density && other.timestep == timestep &&
           other.gravity == gravity && other.visualizer == visualizer && other.invalidate == invalidate &&
           other.entities == entities && other.sensors == sensors;
}

bool FluidSolver::SimulationParameters::operator!=(const FluidSolver::SimulationParameters& other) const
{
    return !(*this == other);
}

void FluidSolver::Simulation::check_for_initialization()
{
    if (internal_parameters != parameters)
    {
        initialize();
    }
}

void FluidSolver::Simulation::execute_simulation_step()
{

    check_for_initialization();

    FLUID_ASSERT(internal_parameters.fluid_solver != nullptr);
    FLUID_ASSERT(internal_parameters.collection != nullptr);
    FLUID_ASSERT(internal_parameters.timestep != nullptr);
    FLUID_ASSERT(internal_parameters.rest_density > 0.0f);
    FLUID_ASSERT(internal_parameters.particle_size > 0.0f);


    // calculate timestep
    internal_parameters.timestep->calculate_current_timestep();
    float current_timestep = internal_parameters.timestep->get_current_timestep();
    timepoint.system_time = std::chrono::system_clock::now();
    timepoint.current_time_step = current_timestep;


    // simulate
    internal_parameters.fluid_solver->execute_simulation_step(current_timestep);

    // simulate entities
    for (auto ent : internal_parameters.entities)
    {
        ent->execute_simulation_step(current_timestep);
    }

    // update simulation time
    timepoint.simulation_time += current_timestep;
    timepoint.timestep_number++;

    // measure sensor data
    for (auto sen : internal_parameters.sensors)
    {
        sen->calculate_and_store(timepoint);
    }

    // call the output manager
    parameters.output.timestep_happened();
}

void FluidSolver::Simulation::initialize()
{
    FLUID_ASSERT(parameters.collection != nullptr);
    FLUID_ASSERT(parameters.rest_density > 0.0f);
    FLUID_ASSERT(parameters.particle_size > 0.0f);

    internal_parameters = parameters;
    internal_parameters.invalidate = false;
    parameters.invalidate = false;

    if (internal_parameters.fluid_solver != nullptr)
    {
        internal_parameters.fluid_solver->parameters.particle_size = parameters.particle_size;
        internal_parameters.fluid_solver->parameters.rest_density = parameters.rest_density;
        internal_parameters.fluid_solver->parameters.gravity = parameters.gravity;
        internal_parameters.fluid_solver->collection = parameters.collection;
        internal_parameters.fluid_solver->initialize();

        neigborhood_interface = internal_parameters.fluid_solver->create_neighborhood_interface();
    }

    if (internal_parameters.timestep != nullptr)
    {
        internal_parameters.timestep->parameters.particle_size = internal_parameters.particle_size;
        internal_parameters.timestep->parameters.particle_collection = internal_parameters.collection;
    }

    for (auto ent : internal_parameters.entities)
    {
        FLUID_ASSERT(ent != nullptr);
        ent->sim.collection = internal_parameters.collection;
        ent->sim.gravity = internal_parameters.gravity;
        ent->sim.particle_size = internal_parameters.particle_size;
        ent->sim.neighborhood_interface = &neigborhood_interface;
        ent->initialize();
    }

    for (auto sen : internal_parameters.sensors)
    {
        FLUID_ASSERT(sen != nullptr);
        sen->parameters.simulation_parameters = &internal_parameters;
        sen->parameters.neighborhood_interface = &neigborhood_interface;
        sen->parameters.manager = &internal_parameters.output;
        sen->initialize();
    }

    if (internal_parameters.visualizer != nullptr)
    {
        internal_parameters.visualizer->parameters.collection = internal_parameters.collection;
        internal_parameters.visualizer->parameters.particle_size = internal_parameters.particle_size;
        internal_parameters.visualizer->parameters.rest_density = internal_parameters.rest_density;
        internal_parameters.visualizer->parameters.neighborhood_interface = &neigborhood_interface;
        internal_parameters.visualizer->initialize();
    }

    {
        parameters.output.parameters.sensors = internal_parameters.sensors;
        parameters.output.initialize();
    }
}

void FluidSolver::Simulation::visualize()
{

    check_for_initialization();

    if (internal_parameters.visualizer == nullptr)
        return;

    FLUID_ASSERT(internal_parameters.collection != nullptr);

    internal_parameters.visualizer->render();
}

void FluidSolver::Simulation::manual_initialize()
{
    initialize();
}

FluidSolver::Compatibility FluidSolver::Simulation::check()
{
    Compatibility c;

    if (parameters.collection == nullptr)
    {
        c.add_issue({"Simulation", "Particle collection is null."});
    }

    if (parameters.fluid_solver == nullptr)
    {
        c.add_issue({"Simulation", "Fluid solver is null."});
    }
    else
    {
        c.add_compatibility(parameters.fluid_solver->check());
    }

    if (parameters.timestep == nullptr)
    {
        c.add_issue({"Simulation", "Timestep is null."});
    }
    else
    {
        c.add_compatibility(parameters.timestep->check());
    }

    if (parameters.visualizer != nullptr)
    {
        c.add_compatibility(parameters.visualizer->check());
    }

    for (auto& sensor : parameters.sensors)
    {
        c.add_compatibility(sensor->check());
    }

    return c;
}

const FluidSolver::Timepoint& FluidSolver::Simulation::get_current_timepoint() const
{
    return timepoint;
}