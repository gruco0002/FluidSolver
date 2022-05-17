#include "Simulator.hpp"

namespace FluidSolver {

    void Simulator::execute_simulation_step() {
        initialize_if_required();

        FLUID_ASSERT(data.fluid_solver != nullptr);
        FLUID_ASSERT(data.collection != nullptr);
        FLUID_ASSERT(data.timestep_generator != nullptr);

        FLUID_ASSERT(parameters.rest_density > 0.0f);
        FLUID_ASSERT(parameters.particle_size > 0.0f);


        // calculate timestep
        {
            data.timestep_generator->generate_next_timestep();
            float current_timestep = data.timestep_generator->get_generated_timestep();
            timepoint.system_time = std::chrono::system_clock::now();
            timepoint.desired_time_step = current_timestep;
            timepoint.actual_time_step = current_timestep;
        }

        // simulate
        data.fluid_solver->execute_simulation_step(timepoint);

        // simulate entities
        for (auto ent : data.entities) {
            // TODO: improve entity time step is fixed
            ent->execute_simulation_step(timepoint.actual_time_step);
        }

        // update simulation time
        timepoint.simulation_time += timepoint.actual_time_step;
        timepoint.timestep_number++;

        // measure sensor data
        for (auto sen : data.sensors) {
            sen->calculate_and_store(timepoint);
        }

        // call the output manager
        output->timestep_happened();
    }

    void Simulator::initialize_if_required() {
        FLUID_ASSERT(data.collection != nullptr);
        FLUID_ASSERT(data.fluid_solver != nullptr);
        FLUID_ASSERT(data.timestep_generator != nullptr);

        FLUID_ASSERT(parameters.rest_density > 0.0f);
        FLUID_ASSERT(parameters.particle_size > 0.0f);

        // check if the parameters have changed
        if (parameters.has_data_changed()) {
            parameters.acknowledge_data_change();

            // give the changed parameters to the data modules
            data.fluid_solver->parameters.particle_size = parameters.particle_size;
            data.fluid_solver->parameters.rest_density = parameters.rest_density;
            data.fluid_solver->parameters.gravity = parameters.gravity;
            data.fluid_solver->parameters.notify_that_data_changed();

            data.timestep_generator->parameters.particle_size = parameters.particle_size;

            for (auto ent : data.entities) {
                FLUID_ASSERT(ent != nullptr);

                ent->sim.gravity = parameters.gravity;
                ent->sim.particle_size = parameters.particle_size;
                ent->sim.notify_that_data_changed();

            }

            for (auto sen : data.sensors) {
                FLUID_ASSERT(sen != nullptr);
                sen->simulator_data.manager = output;
                sen->simulator_parameters.gravity = parameters.gravity;
                // sen->parameters.simulation_parameters = &internal_parameters;

                sen->simulator_parameters.notify_that_data_changed();

            }
        }

        // check if any data has changed
        if (data.has_data_changed()) {
            data.acknowledge_data_change();

            data.fluid_solver->data.timestep_generator = data.timestep_generator;
            data.fluid_solver->data.collection = data.collection;
            data.fluid_solver->data.notify_that_data_changed();

            data.timestep_generator->parameters.particle_collection = data.collection;


            // create a new neighborhood interface
            neigborhood_interface = data.fluid_solver->create_neighborhood_interface();


            for (auto ent : data.entities) {
                FLUID_ASSERT(ent != nullptr);
                ent->sim.collection = data.collection;
                ent->sim.neighborhood_interface = neigborhood_interface;

                ent->sim.notify_that_data_changed();

            }

            for (auto sen : data.sensors) {
                FLUID_ASSERT(sen != nullptr);
                sen->simulator_data.neighborhood_interface = neigborhood_interface;
                sen->simulator_data.manager = output;
                sen->simulator_data.collection = data.collection;

                sen->simulator_data.notify_that_data_changed();

            }

            output->parameters.sensors = data.sensors;
        }
    }


    void Simulator::manual_initialize() {
        parameters.notify_that_data_changed();
        data.notify_that_data_changed();

        initialize_if_required();
    }

    Compatibility Simulator::check() {
        Compatibility c;

        if (data.collection == nullptr) {
            c.add_issue({"Simulation", "Particle collection is null."});
        }

        if (data.fluid_solver == nullptr) {
            c.add_issue({"Simulation", "Fluid solver is null."});
        } else {
            c.add_compatibility(data.fluid_solver->check());
        }

        if (data.timestep_generator == nullptr) {
            c.add_issue({"Simulation", "Timestep is null."});
        } else {
            c.add_compatibility(data.timestep_generator->check());
        }

        for (auto& sensor : data.sensors) {
            c.add_compatibility(sensor->check());
        }

        return c;
    }

    const Timepoint& Simulator::get_current_timepoint() const {
        return timepoint;
    }
    Simulator::Simulator() {
        output = std::make_shared<OutputManager>();
    }
} // namespace FluidSolver