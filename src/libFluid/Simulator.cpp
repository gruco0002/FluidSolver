#include "Simulator.hpp"

namespace LibFluid {

    void Simulator::execute_simulation_step() {
        initialize();

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
            sen->execute_timestep(timepoint);
        }
    }

    void Simulator::initialize() {
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

                ent->simulation_data.gravity = parameters.gravity;
                ent->simulation_data.particle_size = parameters.particle_size;
                ent->simulation_data.notify_that_data_changed();
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
                ent->simulation_data.collection = data.collection;
                ent->simulation_data.neighborhood_interface = neigborhood_interface;

                ent->simulation_data.notify_that_data_changed();
            }

            for (auto sen : data.sensors) {
                FLUID_ASSERT(sen != nullptr);
                sen->simulator_data.neighborhood_interface = neigborhood_interface;
                sen->simulator_data.manager = output;
                sen->simulator_data.collection = data.collection;
                sen->simulator_data.fluid_solver = data.fluid_solver;

                sen->simulator_data.notify_that_data_changed();
            }
        }
    }


    void Simulator::manual_initialize() {
        parameters.notify_that_data_changed();
        data.notify_that_data_changed();

        initialize();
    }

    void Simulator::create_compatibility_report(CompatibilityReport& report) {
        initialize();

        report.begin_scope(FLUID_NAMEOF(Simulator));

        if (data.collection == nullptr) {
            report.add_issue("Particle collection is null.");
        }

        if (data.fluid_solver == nullptr) {
            report.add_issue("Fluid solver is null.");
        } else {
            data.fluid_solver->create_compatibility_report(report);
        }

        if (data.timestep_generator == nullptr) {
            report.add_issue("Timestep is null.");
        } else {
            data.timestep_generator->create_compatibility_report(report);
        }

        for (auto& sensor : data.sensors) {
            sensor->create_compatibility_report(report);
        }

        report.end_scope();
    }

    const Timepoint& Simulator::get_current_timepoint() const {
        return timepoint;
    }
    Simulator::Simulator() {
        output = std::make_shared<OutputManager>();
    }
    std::shared_ptr<NeighborhoodInterface> Simulator::get_neighborhood_interface() {
        return neigborhood_interface;
    }
    void Simulator::set_timepoint(const Timepoint& timepoint) {
        this->timepoint = timepoint;
    }
} // namespace LibFluid